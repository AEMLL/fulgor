#include <iostream>
#include <stdio.h>
#include <math.h>

#include <libremidi/libremidi.hpp>
#include "portaudio.h"
#include <readerwriterqueue/readerwriterqueue.h>

#include "ADSR.h"
#include "Delay.h"
#include "Filter.h"
#include "Wavetable.h"
#include "midiutils.h"

#define NUM_SECONDS   (300)
#define SAMPLE_RATE   (44100)
#define FRAMES_PER_BUFFER  (64)
#define NUM_AUDIO_CHANNELS (2)

#ifndef M_PI
#define M_PI  (3.14159265)
#endif

#define TABLE_SIZE   (200)

/*******************************************************************/

// REFACTOR


typedef struct {
    char message[20];
} UserData;

// g prefix denotes global

// readerwriterqueue
moodycamel::ReaderWriterQueue<libremidi::message> q(16);

// Oscillator and Filter objects
Wavetable gOscillator;
Filter gFilter;
Delay gDelay;

// ADSR objects
ADSR gAmplitudeADSR, gFilterADSR;
float gAmplitude = 0.0;

// MIDI
const int kMaxActiveNotes = 16;
int gActiveNotes[kMaxActiveNotes];
int gActiveNoteCount = 0;


// setup
bool setup(void *userData) {
    // midi setup

    // wavetable
    std::vector<float> wavetable;
	const unsigned int wavetableSize = 512;
		
	// Populate a buffer with the first 64 harmonics of a sawtooth wave
	wavetable.resize(wavetableSize);
	for(unsigned int n = 0; n < wavetable.size(); n++) {
		wavetable[n] = 0;
		for(unsigned int harmonic = 1; harmonic <= 48; harmonic++) {
			wavetable[n] += 0.5 * sinf(2.0 * M_PI * (float)harmonic * (float)n / 
								 (float)wavetable.size()) / (float)harmonic;
		}
	}

    // Initialise the wavetable, passing the sample rate and the buffer
	gOscillator.setup(SAMPLE_RATE, wavetable);

	// Initialise the filter
	gFilter.setSampleRate(SAMPLE_RATE);

    // Initialise the echo
    gDelay.setSampleRate(SAMPLE_RATE);
    gDelay.setMaxDelay(0.125);
    gDelay.setLevel(0.7);
    

	// Initialise the ADSR objects
	gAmplitudeADSR.setSampleRate(SAMPLE_RATE);
	gFilterADSR.setSampleRate(SAMPLE_RATE);
    
    return true;
}

auto midiCallback = [](const libremidi::message& message) {
    std::cout << message << std::endl;
    q.try_enqueue(message);
};

// MIDI note on received
void noteOn(int noteNumber, int velocity) 
{
    // Check if we have any note slots left
    if(gActiveNoteCount < kMaxActiveNotes) {
        // Keep track of this note, then play it
        gActiveNotes[gActiveNoteCount++] = noteNumber;
        
        // Map note number to frequency
        float frequency = powf(2.0, (noteNumber - 69)/12.0) * 440.0;
        gOscillator.setFrequency(frequency);
        
        // Map velocity to gAmplitude on a decibel scale
        float decibels = ((velocity/127.0)*40.0)-40.0;
        gAmplitude = powf(10.0, decibels / 20.0);
    
        // TODO: trigger the ADSR envelopes
        gAmplitudeADSR.trigger();
    }
}

// MIDI note off received
void noteOff(int noteNumber)
{
    bool activeNoteChanged = false;
    
    // TODO: work out when you should release the ADSR envelopes within this function!
    
    // Go through all the active notes and remove any with this number
    for(int i = gActiveNoteCount - 1; i >= 0; i--) {
        if(gActiveNotes[i] == noteNumber) {
            // Found a match: is it the most recent note?
            if(i == gActiveNoteCount - 1) {
                activeNoteChanged = true;
            }
    
            // Move all the later notes back in the list
            for(int j = i; j < gActiveNoteCount - 1; j++) {
                gActiveNotes[j] = gActiveNotes[j + 1];
            }
            gActiveNoteCount--;
        }
    }
    
    if(gActiveNoteCount == 0) {
        gAmplitudeADSR.release();
    }
    else if(activeNoteChanged) {
        // Update the frequency but don't retrigger
        int mostRecentNote = gActiveNotes[gActiveNoteCount - 1];
        
        float frequency = powf(2.0, (mostRecentNote - 69)/12.0) * 440.0;
        gOscillator.setFrequency(frequency);
    }
}

// callback function -> render
int render(const void *inputBuffer,
    void *outputBuffer, 
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData)
{
    float *buffer = (float*)outputBuffer;
    unsigned long i;
        
    (void) timeInfo; /* Prevent unused variable warnings. */
    (void) statusFlags;
    (void) inputBuffer;

    libremidi::message message;
    while (q.try_dequeue(message)) {
        std::cout << "msg received" << std::endl;
        std::cout << message << std::endl;
        
        unsigned char messageType = (message[0] & 0xF0) >> 4;
        if (messageType == 0x9) { // Note On
            int noteNumber = message[1];
            int velocity = message[2];

            std::cout << "noteNumber =" << noteNumber << std::endl;
            
            if (velocity == 0) {
                noteOff(noteNumber);
            } else {
                noteOn(noteNumber, velocity);
            }
        } else if (messageType == 0x8) { // Note Off
            int noteNumber = message[1];
            
            noteOff(noteNumber);
        } else if (messageType == 0xB) {
            unsigned char controller = message[1];
            float value = message[2] / 127.0; 

            switch (controller) {
                case (0x14):
                    gAmplitudeADSR.setAttackTime(value);
                    break;
                case (0x15):
                    gAmplitudeADSR.setDecayTime(value);
                    break;
                case (0x16):
                    gAmplitudeADSR.setSustainLevel(value);
                    break;
                case (0x17):
                    gAmplitudeADSR.setReleaseTime(value);
                    break;
            }
        }

        printf("Frequency: %f, Amplitude: %f\n", gOscillator.getFrequency(), gAmplitude);
    }

    // Now calculate the audio for this block
	for(unsigned int n = 0; n < framesPerBuffer; n++) {
		// TODO: get the next value from the ADSR envelope, scaled by the global amplitude
    	float amplitude = gAmplitude * gAmplitudeADSR.process();
    	
		// TODO: set the filter frequency based on its ADSR envelope -- see Lecture 14
		gFilter.setFrequency(1000.0);
        gFilter.setQ(2);
    	
    	// Calculate the output
    	float out = gOscillator.process() * amplitude;
    	out = 0.5 * gFilter.process(out);
        out = gDelay.process(out, 0.125);
    	
    	for(unsigned int channel = 0; channel < NUM_AUDIO_CHANNELS; channel++) {
			// Write the sample to every audio output channel
    		*buffer++ = out;
    	}
    }

    return paContinue;
        
}

// stream finished -> clean-up
static void StreamFinished( void* userData )
{
    UserData *data = (UserData *) userData;
    printf( "Stream Completed: %s\n", data->message );
}

int main(void)
    {
    PaStreamParameters outputParameters;
    PaStream *stream;
    PaError err;
    UserData data;
    int i;

    libremidi::observer obs;
    auto inputs = obs.get_input_ports();
    auto outputs = obs.get_output_ports();

    if (inputs.size() == 0)
    {
        std::cerr << "No available input port.\n";
        return -1;
    }
    if (outputs.size() == 0)
    {
        std::cerr << "No available output port.\n";
        return -1;
    }

    libremidi::midi_in midiin{ 
        libremidi::input_configuration{ .on_message = midiCallback },
        libremidi::midi_in_configuration_for(obs)
    };

    midiin.open_port(inputs[0]);
    if (!midiin.is_port_connected())
    {
        std::cerr << "Could not connect to midi in\n";
        return -1;
    }

    printf("PortAudio Test: output sine wave. SR = %d, BufSize = %d\n", SAMPLE_RATE, FRAMES_PER_BUFFER);

    err = Pa_Initialize();
    if( err != paNoError ) goto error;

    outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
    if (outputParameters.device == paNoDevice) {
        fprintf(stderr,"Error: No default output device.\n");
        goto error;
    }

    outputParameters.channelCount = 2;       /* stereo output */
    outputParameters.sampleFormat = paFloat32; /* 32 bit floating point output */
    outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;

    setup(nullptr);

    err = Pa_OpenStream(
              &stream,
              NULL, /* no input */
              &outputParameters,
              SAMPLE_RATE,
              FRAMES_PER_BUFFER,
              paClipOff,      /* we won't output out of range samples so don't bother clipping them */
              render,
              &data);
    if( err != paNoError ) goto error;

    sprintf( data.message, "No Message" );
    err = Pa_SetStreamFinishedCallback( stream, &StreamFinished );
    if( err != paNoError ) goto error;

    err = Pa_StartStream( stream );
    if( err != paNoError ) goto error;

    printf("Play for %d seconds.\n", NUM_SECONDS );
    Pa_Sleep( NUM_SECONDS * 1000 );

    err = Pa_StopStream( stream );
    if( err != paNoError ) goto error;

    err = Pa_CloseStream( stream );
    if( err != paNoError ) goto error;

    Pa_Terminate();
    printf("Test finished.\n");

    return err;

error:
    Pa_Terminate();
    fprintf( stderr, "An error occurred while using the portaudio stream\n" );
    fprintf( stderr, "Error number: %d\n", err );
    fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
    return err;
    }