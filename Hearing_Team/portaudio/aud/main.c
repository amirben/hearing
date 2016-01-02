#include "portaudio.h"
#include <stdio.h>
#define SAMPLE_RATE 44100
#define NUM_SECONDS 6
#define FRAMES_PER_BUFFER 256
 #define PA_SAMPLE_TYPE  paFloat32
int patestCallback(const void *input,
                                      void *output,
                                      unsigned long frameCount,
                                      const PaStreamCallbackTimeInfo* timeInfo,
                                      PaStreamCallbackFlags statusFlags,
                                      void *userData);
typedef struct
{
	int          frameIndex;  /* Index into sample array. */
     int          maxFrameIndex;
	 float      *recordedSamples;
 }
 paTestData;

int main()
{

	PaStream *stream;
	int err;
	int numDevices;
	int i;
	int j;
	paTestData dat;
	float samparr[SAMPLE_RATE*5];
	const   PaDeviceInfo *deviceInfo;
	PaStreamParameters inp;
	FILE *fp;
	//// end of decleration
	err= Pa_Initialize();
	if( err != paNoError ) printf(  "PortAudio error: %s\n", Pa_GetErrorText( err ) );
	
	
	dat.frameIndex=0;
	dat.maxFrameIndex=	SAMPLE_RATE*5;
	dat.recordedSamples = samparr;
	numDevices = Pa_GetDeviceCount();
	if( numDevices < 0 )
	{
		printf( "ERROR: Pa_CountDevices returned 0x%x\n", numDevices );
		err = numDevices;
		printf(  "PortAudio error: %s\n", Pa_GetErrorText( err ) );
	}


	for(i=0;i<numDevices;i++)
	{
		deviceInfo = Pa_GetDeviceInfo( i );
		printf("%s\n", deviceInfo->name);
		printf("input channels: %d\n",deviceInfo->maxInputChannels);
		printf("default samp rate: %d\n\n",deviceInfo->defaultSampleRate);
	}
    deviceInfo = Pa_GetDeviceInfo( 9 );
	
	printf("%s\n", deviceInfo->name);
	printf("input channels: %d\n\n",deviceInfo->maxInputChannels);
	printf("default samp rate: %d",deviceInfo->defaultSampleRate);
	inp.channelCount=1;
	inp.device=Pa_GetHostApiInfo(deviceInfo->hostApi)->defaultInputDevice;
	inp.sampleFormat=PA_SAMPLE_TYPE;
	inp.hostApiSpecificStreamInfo=NULL; // ze null
	err = Pa_OpenStream(
			&stream,
			&inp, 
			NULL, /* no output */
			SAMPLE_RATE,
			FRAMES_PER_BUFFER,
			paClipOff,      /* we won't output out of range samples so don't bother clipping them */
			patestCallback,
			&dat );
    if( err != paNoError ) printf(  "PortAudio error: %s\n", Pa_GetErrorText( err ) );

	err = Pa_StartStream( stream );
	if( err != paNoError ) printf(  "PortAudio error: %s\n", Pa_GetErrorText( err ) );

	/* Sleep for several seconds. */
	Pa_Sleep(NUM_SECONDS*1000);

	err = Pa_StopStream( stream );
	if( err != paNoError )  printf(  "PortAudio error: %s\n", Pa_GetErrorText( err ) );

	err = Pa_Terminate();
	if( err != paNoError ) printf(  "PortAudio error: %s\n", Pa_GetErrorText( err ) );

	printf("%d samples taken for a total of %f seconds",dat.frameIndex,(float)dat.frameIndex/SAMPLE_RATE);
	fp=fopen("samp.txt","w");
	j=0;
	while(dat.recordedSamples[j++]==(float)0);

	for(;j<dat.maxFrameIndex;j++)
	{
		//printf("%f\n",dat.recordedSamples[j]);
		fprintf(fp,"%f,",dat.recordedSamples[j]);
	}

	fclose(fp);
	return 0;
}

int patestCallback(const void *input,void *output,unsigned long frameCount,const PaStreamCallbackTimeInfo* timeInfo,PaStreamCallbackFlags statusFlags,void *userData)
{
	unsigned int i=0;
	paTestData *d = (paTestData*)userData;
	float *in = (float*)input;
	const float *rptr = (const float*)input;
	float *wptr = &d->recordedSamples[d->frameIndex];

	if(d->frameIndex>d->maxFrameIndex) return 0;
	while(i<frameCount && (d->frameIndex < d->maxFrameIndex))
	{
		*wptr++=*in++;
		d->frameIndex++;
		i++;
	}
	return 0;
}
