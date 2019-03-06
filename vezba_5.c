#include "vezba_5.h"
#include "graphics.h"

pthread_cond_t deinitCond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t deinitMutex = PTHREAD_MUTEX_INITIALIZER;

//IDirectFBSurface* primary = NULL;
//IDirectFB* dfbInterface = NULL;
int screenWidth = 0;
int screenHeight = 0;
DFBSurfaceDescription surfaceDesc;

int main(int8_t argc, char** argv)
{

	char mod[8];
	char aType[100];
	char vType[100];

	FILE* fp;

	if(argc > 1)
	{
		fp = fopen(argv[1], "r");
	}
	else
	{
		fp = fopen("input.txt", "r");
	}


	rewind(fp);
	while(!feof(fp))
	{
		if(fscanf(fp, "FREQUENCY:%d\n\
					   BANDWIDTH:%d\n\
					   MODULE:%s\n\
					   AUDIO_PID:%d\n\
					   VIDEO_PID:%d\n\
					   AUDIO_TYPE:%s\n\
					   VIDEO_TYPE:%s\n\
					   PROGRAM_NUMBER:%d", &channelI.frequency, &channelI.bandwidth, mod, &channelI.audioPid, &channelI.videoPid, aType, vType, &channelI.programNumber))
		{
			break;
		}
	}

	if(channelI.audioPid == 103 && channelI.videoPid == 101 && channelI.programNumber == 1) 
	{
		printf("PROGRAM 1 IS ON!\n");
	}
	else if(channelI.audioPid == 203 && channelI.videoPid == 201 && channelI.programNumber == 2)
	{
		printf("PROGRAM 2 IS ON!\n");
	}
	else if(channelI.audioPid == 1003 && channelI.videoPid == 1001 && channelI.programNumber == 3)
	{
		printf("PROGRAM 3 IS ON!\n");
	}
	else if(channelI.audioPid == 1503 && channelI.videoPid == 1501 && channelI.programNumber == 4)
	{
		printf("PROGRAM 4 IS ON!\n");
	}
	else if(channelI.audioPid == 2001 && channelI.videoPid == -1 && channelI.programNumber == 5)
	{
		printf("RADIO 1 IS ON!\n");
	}
	else if(channelI.audioPid == 2011 && channelI.videoPid == -1 && channelI.programNumber == 6)
	{
		printf("RADIO 2 IS ON!\n");
	}
	else if(channelI.audioPid == 2021 && channelI.videoPid == -1 && channelI.programNumber == 7)
	{
		printf("RADIO 3 IS ON!\n");
	}
	else
	{
		printf("MUST HAVE:\n\t\
	Channel1: AudioPid = 103; VideoPid = 101\n\t\
	Channel2: AudioPid = 203; VideoPid = 201\n\t\
	Channel3: AudioPid = 1003; VideoPid = 1001\n\t\
	Channel4: AudioPid = 1503; VideoPid = 1501\n\t\
	Radio1: AudioPid = 2001; VideoPid = -1\n\t\
	Radio2: AudioPid = 2011; VideoPid = -1\n\t\
	Radio3: AudioPid = 2021; VideoPid = -1");
	}

	char2Type(mod);
	char2Type(aType);
	char2Type(vType);
	
	printf("freq: %d\n", channelI.frequency);
	printf("band: %d\n", channelI.bandwidth);
	printf("mod: %d\n", channelI.module);
	printf("aPid: %d\n", channelI.audioPid);
	printf("vPid: %d\n", channelI.videoPid);
	printf("aType: %d\n", channelI.audioType);
	printf("vType: %d\n", channelI.videoType);
	printf("programNum: %d\n", channelI.programNumber);

	fclose(fp);

	channelBuffer[3] = '\0';

    /* initialize remote controller module */
    ERRORCHECK(remoteControllerInit());

	DirectFBInit(NULL, NULL);

	DirectFBCreate(&dfbInterface);

	dfbInterface->SetCooperativeLevel(dfbInterface, DFSCL_FULLSCREEN);

	surfaceDesc.flags = DSDESC_CAPS;
	surfaceDesc.caps = DSCAPS_PRIMARY | DSCAPS_FLIPPING;
	dfbInterface->CreateSurface(dfbInterface, &surfaceDesc, &primary);

	primary->GetSize(primary, &screenWidth, &screenHeight);

	IDirectFBFont* fontInterface = NULL;
	DFBFontDescription fontDesc;

	fontDesc.flags = DFDESC_HEIGHT;
	fontDesc.height = 48;

	DFBCHECK(dfbInterface->CreateFont(dfbInterface, "/home/galois/fonts/DejaVuSans.ttf", &fontDesc, &fontInterface));
	DFBCHECK(primary->SetFont(primary, fontInterface));
    
    /* register remote controller callback */
    ERRORCHECK(registerRemoteControllerCallback(remoteControllerCallback));
    
    /* initialize stream controller module */
    ERRORCHECK(streamControllerInit());

    /* wait for a EXIT remote controller key press event */
    pthread_mutex_lock(&deinitMutex);
	if (ETIMEDOUT == pthread_cond_wait(&deinitCond, &deinitMutex))
	{
		printf("\n%s : ERROR Lock timeout exceeded!\n", __FUNCTION__);
	}
	pthread_mutex_unlock(&deinitMutex);
    
    /* unregister remote controller callback */
    ERRORCHECK(unregisterRemoteControllerCallback(remoteControllerCallback));

    /* deinitialize remote controller module */
    ERRORCHECK(remoteControllerDeinit());

    /* deinitialize stream controller module */
    ERRORCHECK(streamControllerDeinit());
  
	primary->Release(primary);
	dfbInterface->Release(dfbInterface);

    return 0;
}

void remoteControllerCallback(uint16_t code, uint16_t type, uint32_t value)
{
    switch(code)
	{
		case KEYCODE_INFO:
            printf("\nInfo pressed\n");          
            if (getChannelInfo(&channelInfo) == SC_NO_ERROR)
            {
                printf("\n********************* Channel info *********************\n");
                printf("Program number: %d\n", channelInfo.programNumber);
                printf("Audio pid: %d\n", channelInfo.audioPid);
                printf("Video pid: %d\n", channelInfo.videoPid);
                printf("**********************************************************\n");
            
				buttonInfo();
			
			}
			break;
		case KEYCODE_P_PLUS:
			printf("\nCH+ pressed\n");
            channelUp();
			break;
		case KEYCODE_P_MINUS:
		    printf("\nCH- pressed\n");
            channelDown();
			break;
		case KEYCODE_EXIT:
			printf("\nExit pressed\n");
            pthread_mutex_lock(&deinitMutex);
		    pthread_cond_signal(&deinitCond);
		    pthread_mutex_unlock(&deinitMutex);
			break;
		case KEYCODE_CHANNEL_1:
			printf("\nCH 1 pressed\n");
            changeChannels(1);
			break;
		case KEYCODE_CHANNEL_2:
			printf("\nCH 2 pressed\n");
            changeChannels(2);
			break;
		case KEYCODE_CHANNEL_3:
			printf("\nCH 3 pressed\n");
            changeChannels(3);
			break;
		case KEYCODE_CHANNEL_4:
			printf("\nCH 4 pressed\n");
            changeChannels(4);
			break;
		case KEYCODE_CHANNEL_5:
			printf("\nCH 5 pressed\n");
            changeChannels(5);
			break;
		case KEYCODE_CHANNEL_6:
			printf("\nCH 6 pressed\n");
            changeChannels(6);
			break;
		case KEYCODE_CHANNEL_7:
			printf("\nCH 7 pressed\n");
            changeChannels(7);
			break;
		case KEYCODE_CHANNEL_8:
			printf("\nCH 8 pressed\n");
            changeChannels(8);
			break;
		case KEYCODE_CHANNEL_9:
			printf("\nCH 9 pressed\n");
            changeChannels(9);
			break;
		case KEYCODE_CHANNEL_0:
			printf("\nCH 0 pressed\n");
            changeChannels(0);
			break;
		default:
			printf("\nPress P+, P-, channel 0-9, info or exit! \n\n");
	}
}

void char2Type (char types[100])
{
	if(strcmp("AUDIO_TYPE_MPEG_AUDIO", types) == 0) 
	{
		channelI.audioType = AUDIO_TYPE_MPEG_AUDIO;
	}

	else if(strcmp("AUDIO_TYPE_MP3", types) == 0) 
	{
		channelI.audioType = AUDIO_TYPE_MP3;
	}

	else if(strcmp("AUDIO_TYPE_DOLBY_AC3", types) == 0) 
	{
		channelI.audioType = AUDIO_TYPE_DOLBY_AC3;
	}

	else if(strcmp("AUDIO_TYPE_HE_AAC", types) == 0) 
	{
		channelI.audioType = AUDIO_TYPE_HE_AAC;
	}

	else if(strcmp("VIDEO_TYPE_MPEG4", types) == 0) 
	{
		channelI.videoType = VIDEO_TYPE_MPEG4;
	}

	else if(strcmp("VIDEO_TYPE_MPEG2", types) == 0) 
	{
		channelI.videoType = VIDEO_TYPE_MPEG2;
	}

	else if(strcmp("VIDEO_TYPE_MPEG1", types) == 0) 
	{
		channelI.videoType = VIDEO_TYPE_MPEG1;
	}

	else if(strcmp("VIDEO_TYPE_H264", types) == 0) 
	{
		channelI.videoType = VIDEO_TYPE_H264;
	}

	else if(strcmp("DVB_T", types) == 0) 
	{
		channelI.module = DVB_T;
	}

	else if(strcmp("DVB_T2", types) == 0) 
	{
		channelI.module = DVB_T2;
	}
	else
	{
		printf("\nNe moze!");
	}

}
