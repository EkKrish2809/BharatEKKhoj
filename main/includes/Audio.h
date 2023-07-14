#pragma once
// OpenAL related headers
#include <stdio.h>
#include <AL/alut.h> // internally includes 'al.h' and 'alc.h'


class Audio
{
public:
	ALCenum error; // error checking of ALUT and OpenAL functions
	ALuint audioBuffer, audioSource;

	//const char *tracks[] = {"trial4.wav","campfire-1.wav","Redwattled_Lapwing.wav","cricket_chirp.wav","sound_of_crickets"};


	int initializeAudio(void)
	{
		// function declarations
		void uninitializeAudio(void);
																												
		// code
		alutInit(NULL, NULL);
		error = alutGetError();
		if (error != ALUT_ERROR_NO_ERROR)
		{
			printf("alutInit() Failed.\n");
			uninitializeAudio();
			return(-1);
		}
																											
		audioBuffer = alutCreateBufferFromFile("./assets/music/MereSajanUssPar.wav");
		error = alutGetError();
		if (error != ALUT_ERROR_NO_ERROR)
		{
			printf("alutCreateBufferFromFile() Failed : Creation of Audio Buffer Failed. \n");
			uninitializeAudio();
			return(-2);
		}
																											
		alGenSources(1, &audioSource);
		error = alGetError();
		if (error != AL_NO_ERROR)
			{
			printf("alGenSources() Failed : Generationg Audio Sources Failed.\n");
			uninitializeAudio();
			return(-3);
		}
																											
		alSourcei(audioSource, AL_BUFFER, audioBuffer);
		error = alGetError();
		if (error != AL_NO_ERROR)
		{
			printf("alSourcei() Failed : Attaching Audio Buffer to Audio Source Failed.\n");
			uninitializeAudio();
			return(-4);
		}
																												
		alSourcePlay(audioSource);
		error = alGetError();
		if (error != AL_NO_ERROR)
		{
			printf("alSourcePlay() Failed.\n");
			uninitializeAudio();
			return(-5);
		}
		return(0);
		
		
	}


	void uninitializeAudio(void)
	{
		// variable declarations
		ALint state;

		// code
		alGetSourcei(audioSource, AL_SOURCE_STATE, &state);
		if (state == AL_PLAYING)
		{
			alSourceStop(audioSource);
			error = alGetError();
			if (error != AL_NO_ERROR)
			{
				printf("alSourceStop() Failed..\n");
			}
		}
																										
		alSourcei(audioSource, AL_BUFFER, 0);
		error = alGetError();
		if (error != AL_NO_ERROR)
		{
			printf("alSourcei() Failed : Attaching Audio Buffer To Audio Source Failed.\n");
		}
																										
		alDeleteSources(1, &audioSource);
		error = alGetError();
		if (error != AL_NO_ERROR)
		{
			printf("alDeleteSource() Failed : DeletingAudio Source Failed.\n");
		}
		else
			audioSource = 0;
																										
		alDeleteBuffers(1, &audioBuffer);
		error = alGetError();
		if (error != AL_NO_ERROR)
		{
			printf("alDeleteBuffers() Failed : Deleting Audio Buffer Failed.\n");
		}
		else
			audioBuffer = 0;
																										
		alutExit();
		
		
	}

};
