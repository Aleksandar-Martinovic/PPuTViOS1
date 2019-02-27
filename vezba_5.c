#include "remote_controller.h"
#include "stream_controller.h"

static inline void textColor(int32_t attr, int32_t fg, int32_t bg)
{
   char command[13];

   /* command is the control command to the terminal */
   sprintf(command, "%c[%d;%d;%dm", 0x1B, attr, fg + 30, bg + 40);
   printf("%s", command);
}

/* macro function for error checking */
#define ERRORCHECK(x)                                                       \
{                                                                           \
if (x != 0)                                                                 \
 {                                                                          \
    textColor(1,1,0);                                                       \
    printf(" Error!\n File: %s \t Line: <%d>\n", __FILE__, __LINE__);       \
    textColor(0,7,0);                                                       \
    return -1;                                                              \
 }                                                                          \
}

static void remoteControllerCallback(uint16_t code, uint16_t type, uint32_t value);
static pthread_cond_t deinitCond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t deinitMutex = PTHREAD_MUTEX_INITIALIZER;
static ChannelInfo channelInfo;
static void char2Type (char* types);

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

    /* initialize remote controller module */
    ERRORCHECK(remoteControllerInit());
    
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

	if(strcmp("AUDIO_TYPE_MP3", types) == 0) 
	{
		channelI.audioType = AUDIO_TYPE_MP3;
	}

	if(strcmp("AUDIO_TYPE_DOLBY_AC3", types) == 0) 
	{
		channelI.audioType = AUDIO_TYPE_DOLBY_AC3;
	}

	if(strcmp("AUDIO_TYPE_HE_AAC", types) == 0) 
	{
		channelI.audioType = AUDIO_TYPE_HE_AAC;
	}

	if(strcmp("VIDEO_TYPE_MPEG4", types) == 0) 
	{
		channelI.videoType = VIDEO_TYPE_MPEG4;
	}

	if(strcmp("VIDEO_TYPE_MPEG2", types) == 0) 
	{
		channelI.videoType = VIDEO_TYPE_MPEG2;
	}

	if(strcmp("VIDEO_TYPE_MPEG1", types) == 0) 
	{
		channelI.videoType = VIDEO_TYPE_MPEG1;
	}

	if(strcmp("VIDEO_TYPE_H264", types) == 0) 
	{
		channelI.videoType = VIDEO_TYPE_H264;
	}

	if(strcmp("DVB_T", types) == 0) 
	{
		channelI.module = DVB_T;
	}

	if(strcmp("DVB_T2", types) == 0) 
	{
		channelI.module = DVB_T2;
	}
	else
	{
		printf("Ne moze!");
	}

}
