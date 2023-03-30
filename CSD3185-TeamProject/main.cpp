#include <pocketsphinx.h>
#include <string>
#include <iostream>

ps_decoder_t* ps;
cmd_ln_t* config;
FILE* fh;
char const* hyp, * uttid;
int16 buf[512];
int rv;
int32 score;
std::string audiofilepath = "model/wav/speaker_";
//for graphs
int num_frames = 0;
int32 max_amplitude = 0;
int successCounter = 0;

void recogniseFromFile(std::string audiofilepath, std::string correctPhrase) {
    fopen_s(&fh, audiofilepath.c_str(), "rb");
    if (fh == NULL) {
        fprintf(stderr, "Unable to open input file goforward.raw\n");
        //return -1;
    }

    rv = ps_start_utt(ps);

    while (!feof(fh)) {
        size_t nsamp;
        nsamp = fread(buf, 2, 512, fh);
        rv = ps_process_raw(ps, buf, nsamp, FALSE, FALSE);

        // Keep track of the number of frames processed
        num_frames++;

        // Calculate the maximum amplitude of the audio data
        for (int i = 0; i < nsamp; i++) {
            if (abs(buf[i]) > max_amplitude) {
                max_amplitude = abs(buf[i]);
            }
        }


    }

    rv = ps_end_utt(ps);
    if (ps_get_hyp(ps, NULL) == NULL) {
        hyp = " ";
        std::cout << "here\n";
    }
    else {
        hyp = ps_get_hyp(ps, NULL);
    }
    printf("Recognized: %s\n", hyp);

    // Print out some basic audio analysis
    printf("Number of frames: %d\n", num_frames);
    printf("Maximum amplitude: %d\n", max_amplitude);
    num_frames = 0;
    max_amplitude = 0;
    if (hyp == correctPhrase)
        ++successCounter;



    fclose(fh);
}
int main(int argc, char* argv[])
{
    config = cmd_ln_init(NULL, ps_args(), TRUE,
        "-hmm",  "model/model_parameters/demo.ci_cont",
        //"-lm",  "model/etc/demo.lm",                      //comment this line and uncomment the line below if want to use keyword search 
        "-kws", "model/keywords.list",
        "-dict",  "model/etc/demo.dic",
        "-logfn", "model/log",
        "-cmn", "none",
        "-cmninit", "50.0",
        NULL);
    if (config == NULL) {
        fprintf(stderr, "Failed to create config object, see log for details\n");
        return -1;
    }

    ps = ps_init(config);
    if (ps == NULL) {
        fprintf(stderr, "Failed to create recognizer, see log for details\n");
        return -1;
    }
    for (int i = 1; i < 196; ++i)
    {
        //speaker 1 to 90 -> contains audio files for jbrf hsmy hmsz (1,2,3)
        for (int j = 1; j < 8; ++j) {

            if (i < 91 && j > 3) continue;
            if (i > 90 && i < 146 && (j < 4 || j > 6)) continue;
            if (i > 145 && j < 7) continue;

            audiofilepath = "model/wav/speaker_" + std::to_string(i) + "/" + std::to_string(i) + "_0" + std::to_string(j) + ".wav";
            std::cout << audiofilepath << std::endl;
            std::string correctPhrase = "";
            if (j == 1) correctPhrase = "JIANBURUFEI";
            if (j == 2) correctPhrase = "HUNSHUIMOYU";
            if (j == 3) correctPhrase = "HUOMAOSANZHANG";
            if (j == 4) correctPhrase = "JINGANGBUHUAI";
            if (j == 5) correctPhrase = "YAQUEWUSHENG";
            if (j == 6) correctPhrase = "ZHIMAKAIMEN";
            if (j == 7) correctPhrase = "GONGXIFACAI";
            recogniseFromFile(audiofilepath, correctPhrase);
        }
  
    }
    std::cout << "Number of successful recognitions : " << successCounter << "/485\n";
    ps_free(ps);
    cmd_ln_free_r(config);


    return 0;
}