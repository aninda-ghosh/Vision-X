/******************************************************Text-To-Read*********************************************************/
/**FROM speak_lib.h :

    espeak_initialize()
    1. output: the audio data can either be played by eSpeak or passed back by the SynthCallback function.
    2. Buflength:  The length in mS of sound buffers passed to the SynthCallback function.
    3. options: bit 0: 1=allow espeakEVENT_PHONEME events.
    4. path: The directory which contains the espeak-data directory, or NULL for the default location.

    espeak_Synth()
    1. text: The text to be spoken, terminated by a zero character. It may be either 8-bit characters,wide characters (wchar_t), or UTF8 encoding.  Which of these is determined by the "flags"parameter.
    2. Size: Equal to (or greatrer than) the size of the text data, in bytes.  This is used in order to allocate internal storage space for the text.  This value is not used forAUDIO_OUTPUT_SYNCHRONOUS mode.
    3. position:  The position in the text where speaking starts. Zero indicates speak from the start of the text.
    4. position_type:  Determines whether "position" is a number of characters, words, or sentences.
    5. Values:
    6. end_position:  If set, this gives a character position at which speaking will stop.  A value of zero indicates no end position.
    7. flags:  These may be OR'd together:
            Type of character codes, one of:
                espeakCHARS_UTF8     UTF8 encoding
                espeakCHARS_8BIT     The 8 bit ISO-8859 character set for the particular language.
                espeakCHARS_AUTO     8 bit or UTF8  (this is the default)
                espeakCHARS_WCHAR    Wide characters (wchar_t)
                espeakSSML   Elements within < > are treated as SSML elements, or if not recognised are ignored.
                espeakPHONEMES  Text within [[ ]] is treated as phonemes codes (in espeak's Hirshenbaum encoding).
                espeakENDPAUSE  If set then a sentence pause is added at the end of the text.  If not set then this pause is suppressed.
    8. unique_identifier: message identifier; helpful for identifying later data supplied to the callback.
    9. user_data: pointer which will be passed to the callback function.

**/


void SetSound()
{
    output = AUDIO_OUTPUT_PLAYBACK;
    espeak_Initialize(output, Buflength, path, Options);
    espeak_SetVoiceByName(Voice);
}

///Function to be Called when human is detected
void humandetect(int net)
{
    if(net == 1)
    {
        text = {"human detected"};
        TextSize = strlen(text)+1;
        espeak_Synth(text, TextSize, position, position_type, end_position, flags, unique_identifier, user_data );
        espeak_Synchronize();
        cout<<"Human Detected"<<endl;
    }
    else if(net == 0)
    {
        text = {""};
        TextSize = strlen(text)+1;
        espeak_Synth(text, TextSize, position, position_type, end_position, flags, unique_identifier, user_data );
        espeak_Synchronize();
        cout<<"Nothing"<<endl;
    }
}

///Function to be Called when door is detected
void doordetect(int net)
{
    if(net == 1)
    {
        text = {"door ahead"};
        TextSize = strlen(text)+1;
        espeak_Synth(text, TextSize, position, position_type, end_position, flags, unique_identifier, user_data );
        espeak_Synchronize();
    }
    else
    {
        text = {" "};
        TextSize = strlen(text)+1;
        espeak_Synth(text, TextSize, position, position_type, end_position, flags, unique_identifier, user_data );
        espeak_Synchronize();
    }
}

///Function to be Called when Stair is detected
void stairdetect(int net)
{
    if(net == 1)
    {
        text = {"Stair ahead"};
        TextSize = strlen(text)+1;
        espeak_Synth(text, TextSize, position, position_type, end_position, flags, unique_identifier, user_data );
        espeak_Synchronize();
    }
    else
    {
        text = {" "};
        TextSize = strlen(text)+1;
        espeak_Synth(text, TextSize, position, position_type, end_position, flags, unique_identifier, user_data );
        espeak_Synchronize();
    }
}

///Speak function to generate speak
void speak(int d,int s,int e)///s is obtained from the decision taken in segmentation part
{
    if(d <= 43)
    {
        text = {"You do not have, a clear ground, for movement"};
        TextSize = strlen(text)+1;
        espeak_Synth(text, TextSize, position, position_type, end_position, flags, unique_identifier, user_data );
        espeak_Synchronize();
    }
    else if(d >43)
    {
        text = {"You have, a clear Ground"};
        TextSize = strlen(text)+1;
        espeak_Synth(text, TextSize, position, position_type, end_position, flags, unique_identifier, user_data );
        espeak_Synchronize();
        if( ((s+e)/2) > 210)
        {
            text = {"Go, by your right side"};
            TextSize = strlen(text)+1;
            espeak_Synth(text, TextSize, position, position_type, end_position, flags, unique_identifier, user_data );
            espeak_Synchronize();
        }
        else if( ((s+e)/2) < 110 )
        {
            text = {"Go, by your left side"};
            TextSize = strlen(text)+1;
            espeak_Synth(text, TextSize, position, position_type, end_position, flags, unique_identifier, user_data );
            espeak_Synchronize();
        }
        else
        {
            text = {"Go, Forward"};
            TextSize = strlen(text)+1;
            espeak_Synth(text, TextSize, position, position_type, end_position, flags, unique_identifier, user_data );
            espeak_Synchronize();
        }
    }

}
