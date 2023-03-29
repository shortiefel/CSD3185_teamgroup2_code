#include <pocketsphinx.h>

int main(int argc, char* argv[])
{
    ps_decoder_t* ps;
    cmd_ln_t* config;
    FILE* fh;
    char const* hyp, * uttid;
    int16 buf[512];
    int rv;
    int32 score;

    //for graphs
    int num_frames = 0;
    int32 max_amplitude = 0;

    config = cmd_ln_init(NULL, ps_args(), TRUE,
        "-hmm",  "model/en-us/en-us",
        "-lm",  "model/en-us/en-us.lm.bin",
        "-dict",  "model/en-us/cmudict-en-us.dict",
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

    fopen_s(&fh, "model/goforward.raw", "rb");
    if (fh == NULL) {
        fprintf(stderr, "Unable to open input file goforward.raw\n");
        return -1;
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
    hyp = ps_get_hyp(ps, &score);
    printf("Recognized: %s\n", hyp);


    // Print out some basic audio analysis
    printf("Number of frames: %d\n", num_frames);
    printf("Maximum amplitude: %d\n", max_amplitude);


    fclose(fh);
    ps_free(ps);
    cmd_ln_free_r(config);


    return 0;
}