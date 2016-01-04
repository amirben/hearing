#include "portaudio.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define SAMPLE_RATE 44100
#define NUM_SECONDS 5
#define FRAMES_PER_BUFFER 256
 #define PA_SAMPLE_TYPE  paFloat32

int patestCallback(const void *input,
                                      void *output,
                                      unsigned long frameCount,
                                      const PaStreamCallbackTimeInfo* timeInfo,
                                      PaStreamCallbackFlags statusFlags,
                                      void *userData);
int patestCallback_2ch(const void *input,
                                      void *output,
                                      unsigned long frameCount,
                                      const PaStreamCallbackTimeInfo* timeInfo,
                                      PaStreamCallbackFlags statusFlags,
                                      void *userData);
float RMS(float *data, int len);
typedef struct
{
	int          frameIndex;  /* Index into sample array. */
     int          maxFrameIndex;//maximum samples available
	 float      *recordedSamples_ch1;//sampling for channel 1
	 float      *recordedSamples_ch2;//sampling for channel 2
 }
 paTestData;

int main()
{

	PaStream *stream;
	int err;//basic error chcking by portaudio
	int numDevices;//amount of devices recognized by portaudio
	int i;//for loops
	int j;//for loops
	int device_chosen;//number of device chosen
	int channel_chosen;//amount of channels to be recorded into
	paTestData dat;//struct sent to callback function
	char *bin;
	const   PaDeviceInfo *deviceInfo;
	float rms=0;
	PaStreamParameters inp;//input into stream
	FILE *fp;//for file I/O



	//// end of decleration


	err= Pa_Initialize();
	if( err != paNoError ) printf(  "PortAudio error: %s\n", Pa_GetErrorText( err ) );
	
	
	dat.frameIndex=0;
	dat.maxFrameIndex=	SAMPLE_RATE*NUM_SECONDS;
	dat.recordedSamples_ch1 = (float*)calloc(SAMPLE_RATE*NUM_SECONDS,sizeof(float));
	dat.recordedSamples_ch2 = (float*)calloc(SAMPLE_RATE*NUM_SECONDS,sizeof(float));
	numDevices = Pa_GetDeviceCount();
	if( numDevices < 0 )
	{
		printf( "ERROR: Pa_CountDevices returned 0x%x\n", numDevices );
		err = numDevices;
		printf(  "PortAudio error: %s\n", Pa_GetErrorText( err ) );
	}


	for(i=0;i<numDevices;i++)		//loops over all devices showing basic information
	{
		deviceInfo = Pa_GetDeviceInfo( i );
		printf(" device number %d :\n",i);
		printf("%s\n", deviceInfo->name);
		printf("input channels: %d\n",deviceInfo->maxInputChannels);
		printf("default samp rate: %p \n\n",deviceInfo->defaultSampleRate); //shows sample rate of 0 all the time(though recording works)
	}

	//prompting user for info
	printf("choose your device:");
	scanf("%d",&device_chosen);
	deviceInfo = Pa_GetDeviceInfo( device_chosen );
	printf("choose input channels:");
	scanf("%d",&channel_chosen);
	//
	inp.channelCount=channel_chosen;
	inp.device=Pa_GetHostApiInfo(deviceInfo->hostApi)->defaultInputDevice;
	inp.sampleFormat=PA_SAMPLE_TYPE;
	inp.hostApiSpecificStreamInfo=NULL; // optional for future implementation

	err = Pa_OpenStream(
			&stream,
			&inp, 
			NULL, /* no output */
			SAMPLE_RATE,
			FRAMES_PER_BUFFER,
			paClipOff,      /* we won't output out of range samples so don't bother clipping them */
			patestCallback, //callback function to be run each time samples are available
			&dat );//data to be passed to function, currently struct of type patestdata
    if( err != paNoError ) printf(  "PortAudio error: %s\n", Pa_GetErrorText( err ) );


	err = Pa_StartStream( stream );
	if( err != paNoError ) printf(  "PortAudio error: %s\n", Pa_GetErrorText( err ) );
	
	//just waiting for audio to come in
	/* Sleep for several seconds. */
	Pa_Sleep((NUM_SECONDS+1)*1000);


	err = Pa_StopStream( stream );
	if( err != paNoError )  printf(  "PortAudio error: %s\n", Pa_GetErrorText( err ) );

	err = Pa_Terminate();
	if( err != paNoError ) printf(  "PortAudio error: %s\n", Pa_GetErrorText( err ) );
	//closed all portaudio streams and such
	//not using anything portaudio related from now on

	//only output
	//currently works on only 1 channel

	printf("%d samples taken for a total of %f seconds",dat.frameIndex,(float)dat.frameIndex/SAMPLE_RATE);
	fp=fopen("samp.csv","w");//writes to csv file
	j=0;
	while(dat.recordedSamples_ch1[j++]==(float)0);	//skip leading zeroes(alot of them)
	j+=9247;										//there's a lot of low level noise from there on
	for(;j<dat.maxFrameIndex;j++)
	{
		fprintf(fp,"%f,",dat.recordedSamples_ch1[j]/*,dat.recordedSamples_ch2[j]*/);
	}

	fclose(fp);
	j=0;
	while(dat.recordedSamples_ch1[j++]==(float)0);	//skip leading zeroes(alot of them)
	j+=9247;										//there's a lot of low level noise from there on
	fp=fopen("samp.bin","w");						//write pure float memory to bytes
	dat.recordedSamples_ch1+=j;
	rms=RMS(dat.recordedSamples_ch1,dat.maxFrameIndex-j);//calculates rms
	printf("\n\n");
	printf("rms is: %f \n\n",rms);
	bin= (char*)dat.recordedSamples_ch1;
	for(;j<dat.maxFrameIndex;j++)//loops over all array and outputs each float as 4 chars into file
	{
		fputc(*bin++,fp);
		fputc(*bin++,fp);
		fputc(*bin++,fp);
		fputc(*bin++,fp);
	}

	return 0;
}

int patestCallback(const void *input,void *output,unsigned long frameCount,const PaStreamCallbackTimeInfo* timeInfo,PaStreamCallbackFlags statusFlags,void *userData)
{
	unsigned int i=0;
	paTestData *d = (paTestData*)userData;
	float *in = (float*)input;
	const float *rptr = (const float*)input;
	float *wptr = &d->recordedSamples_ch1[d->frameIndex];

	if(d->frameIndex>d->maxFrameIndex) return 0;
	while(i<frameCount && (d->frameIndex < d->maxFrameIndex))
	{
		*wptr++=*in++;
		d->frameIndex++;
		i++;
	}
	return 0;
}

float RMS(float *data, int len)
{
	float rms = 0;
	int i;
	for(i = 0; i < len; i++)
	{
		rms += data[i]*data[i];
	}
	rms /= (float)len;
	rms = (float)sqrt(rms);
	return rms;
}

int patestCallback_2ch(const void *input,void *output,unsigned long frameCount,const PaStreamCallbackTimeInfo* timeInfo,PaStreamCallbackFlags statusFlags,void *userData)
{
	unsigned int i=0;
	paTestData *d = (paTestData*)userData;
	float *in = (float*)input;
	const float *rptr = (const float*)input;
	float *wptr1 = &d->recordedSamples_ch1[d->frameIndex];
	float *wptr2 = &d->recordedSamples_ch2[d->frameIndex];

	if(d->frameIndex>d->maxFrameIndex) return 0;
	while(i<frameCount && (d->frameIndex < d->maxFrameIndex))
	{
		*wptr1++=*in++;
		*wptr2++=*in++;
		d->frameIndex++;
		i++;
	}
	return 0;
}

