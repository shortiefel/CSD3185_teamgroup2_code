/******************************************************************************/
/*!
\file             main.cpp
\author     Lee Yu Ting
                    Lim Yi Qin
                    Leong Jia Yi Celine
                    Tan Wei Ling Felicia
                    Woon Ting Ting

\param      ML - Team 
\brief          The program that initializes the PocketSphinx decoder and loops through all audio
                     files to perform recognition and analysis.

                     This also includes the different type of anaylsis done such as:
                      - Word Error Rate 
                      - Time Analysis
                      - Amplitude Analysis

  Copyright (C) 2022 DigiPen Institure of Technology.
  Reproduction or disclosure of this file or its contents
  without the prior written consent of DigiPen Institute of
  Technology is prohibited.
*/
/******************************************************************************/

#include <pocketsphinx.h>
#include <string>
#include <iostream>
#include <ctime>
#include <vector>
#include <sstream>
#include <fstream>

/**
* Global Variables
*/
ps_decoder_t* ps;
cmd_ln_t* config;
FILE* fh;
char const* hyp, * uttid;
int16 buf[512];
int rv;
int32 score;
std::string audiofilepath = "model/wav/speaker_";
FILE* fptr;


/**
* Frame rate, Amplitude and Success Count Analysis
*/
int num_frames = 0;
int32 max_amplitude = 0;
int successCounter = 0;
int num_audio_files = 0;
int num_insertions = 0;
int num_substitutions = 0;
int num_deletions = 0;
double total_time_taken = 0.0;

/**
* @brief Recognizes speech from an audio file and performs analysis on the recognition.
*
* @param audiofilepath The file path of the audio file to recognize.
* @param correctPhrase The correct phrase to be recognized from the audio file.
*
* @return None.
*/
void recogniseFromFile(std::string audiofilepath, std::string correctPhrase) 
{

    //Start measuring time taken by the system
    clock_t startTime = clock();

    //opening of audio file
    fopen_s(&fh, audiofilepath.c_str(), "rb");
    if (fh == NULL) {
        fprintf(stderr, "Unable to open input file goforward.raw\n");
        return;
    }

    // Start the utterance
    rv = ps_start_utt(ps);

    // Process the audio file
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

    // End the utterance
    rv = ps_end_utt(ps);

    //Time Anaylsis time taken by the system for audio file 
    clock_t endTime = clock();
    double time_taken = double(endTime - startTime) / CLOCKS_PER_SEC;
    total_time_taken += time_taken;
    printf("Time taken: %.2fs\n", time_taken);
    fprintf(fptr,"Time taken: %.2fs\n", time_taken);

    // Get the recognized phrase
    if (ps_get_hyp(ps, NULL) == NULL) 
    {
        hyp = " ";
    }
    else 
    {
        hyp = ps_get_hyp(ps, NULL);
    }

    // Print the ground truth and recognized phrase
    printf("Ground truth: %s\n", correctPhrase.c_str());
    printf("Recognized phrase: %s\n", hyp);

    fprintf(fptr, "Ground truth: %s\n", correctPhrase.c_str());
    fprintf(fptr, "Recognized phrase: %s\n", hyp);

    // Split the ground truth and recognized phrases into words
    std::istringstream correctPhraseStream(correctPhrase);
    std::istringstream hypStream(hyp);
    std::vector<std::string> correctWords{}, hypWords{};
    std::string word;
    while (correctPhraseStream >> word) {
        correctWords.push_back(word);
    }
    while (hypStream >> word) {
        hypWords.push_back(word);
    }


    // Calculate the word error rate (WER)
    // WER = (I + S + D) / N
    int insertions = 0;
    int deletions = 0;
    int substitutions = 0;

    // Deletions
    if (hypWords.size() == 0) {
        ++deletions;
    }

    // Substiutions
    if (hypWords.size() == correctWords.size() && hypWords[0] != correctWords[0]) {
        ++substitutions;
    }

    // Insertions

    insertions = hypWords.size() - correctWords.size();
    int numCorrectWords = 0;
    bool found = false;
    if (insertions != 0) {
        // Check if any of the words in hyp matches correctPhrase
        for (int i = 0; i < hypWords.size(); ++i) {
            for (int j = 0; j < correctWords.size(); ++j) {
                if (correctWords[j] == hypWords[i]) {
                    found = true;
                    break;
                }
            }
        }

        if (found == false) {
            ++insertions;
        }
    }


    double wer = 1;
    if (hypWords.size() != 0)
        wer = ((double)(insertions + substitutions + deletions) / (double)hypWords.size());
    printf("Word error rate: %.2f%%\n", wer * 100.0);
    fprintf(fptr, "Word error rate: %.2f%%\n", wer * 100.0);

    //Compare the recognized phrase with ground truth phrases
    if (hyp == correctPhrase) {
        ++successCounter;
    }

    // Frame Rate Anaylsis
    printf("Number of frames: %d\n", num_frames);
    fprintf(fptr, "Number of frames: %d\n", num_frames);

    //Amplitude (wave) Analysis
    printf("Maximum amplitude: %d\n", max_amplitude);
    fprintf(fptr, "Maximum amplitude: %d\n", max_amplitude);

    num_frames = 0;
    max_amplitude = 0;

    fclose(fh);

    ++num_audio_files;
    num_insertions += insertions;
    num_deletions += deletions;
    num_substitutions += substitutions;
}

/**
* @brief Initializes configuration of the recognizer and disables noise removal, iterates
*        over the audio files to be processed.
*
* @return None.
*/
void Without_Remove_Noise() {
    // Initialize the configuration for the PocketSphinx speech recognizer
    config = cmd_ln_init(NULL, ps_args(), TRUE,
        "-hmm", "model/model_parameters/demo.ci_cont",
        "-lm", "model/etc/demo.lm",                    
        //"-kws", "model/keywords.list",
        "-dict", "model/etc/demo.dic",
        "-logfn", "model/log",
        "-cmninit", "56.0",
        "-remove_noise", "no",
        NULL);


    // Check if the configuration was successfully created
    if (config == NULL) {
        fprintf(stderr, "Failed to create config object, see log for details\n");
        return;
    }

    // Initialize the PocketSphinx speech recognizer using the configuration
    ps = ps_init(config);

    // Check if the recognizer was successfully created
    if (ps == NULL) {
        fprintf(stderr, "Failed to create recognizer, see log for details\n");
        return;
    }

    fptr = fopen("./results/without_remove_noise_results.txt", "w");
    fprintf(fptr, "Remove noise: No\n");
    // Loop through all audio files and phrases to recognize speech
    for (int i = 1; i < 26; ++i)
    {
        //speaker 1 to 90 -> contains audio files for jbrf hsmy hmsz (1,2,3)
        for (int j = 1; j < 8; ++j)
        {
            // Create the file path for the current audio file
            audiofilepath = "model/testing_data/speaker_" + std::to_string(i) + "/" + std::to_string(i) + "_0" + std::to_string(j) + ".wav";
            std::cout << "\nAudio file: " << audiofilepath << std::endl;
            fprintf(fptr, "\nAudio file: %s\n", audiofilepath.c_str());
            // Set the correct phrase for the current audio file
            std::string correctPhrase = "";
            if (j == 1) correctPhrase = "JIANBURUFEI";
            if (j == 2) correctPhrase = "HUNSHUIMOYU";
            if (j == 3) correctPhrase = "HUOMAOSANZHANG";
            if (j == 4) correctPhrase = "JINGANGBUHUAI";
            if (j == 5) correctPhrase = "YAQUEWUSHENG";
            if (j == 6) correctPhrase = "ZHIMAKAIMEN";
            if (j == 7) correctPhrase = "GONGXIFACAI";

            // Recognize speech from the current audio file
            recogniseFromFile(audiofilepath, correctPhrase);
        }

    }

    printf("\n\nIn summary (without remove noise):\n");
    fprintf(fptr, "\n\nIn summary (without remove noise):\n");

    // Print number of insertions, substitutions and deletions
    printf("\nInsertions: %d    Substitutions: %d   Deletions: %d\n", num_insertions, num_substitutions, num_deletions);
    fprintf(fptr, "\nInsertions: %d    Substitutions: %d   Deletions: %d\n", num_insertions, num_substitutions, num_deletions);

    // Calculate total WER
    double totalWER = (double)(num_insertions + num_substitutions + num_deletions) / (double)num_audio_files;
    printf("\nTotal Word Error Rate (WER): %.2f%%\n", totalWER * 100.0);
    fprintf(fptr, "\nTotal Word Error Rate (WER): %.2f%%\n", totalWER * 100.0);

    // Print out the number of successful recognitions
    std::cout << "Number of successful recognitions : " << successCounter << "/" << num_audio_files << "\n";
    fprintf(fptr, "Number of successful recognitions : %d/%d\n", successCounter, num_audio_files);

    // Calculate the recognition accuracy
    double accuracy = (double)successCounter / (double)num_audio_files;
    printf("Recognition accuracy: %.2f%%\n", accuracy * 100.0);
    fprintf(fptr, "Recognition accuracy: %.2f%%\n\n", accuracy * 100.0);

    // Print out total time taken
    printf("Total Time taken: %.2fs\n", total_time_taken);
    fprintf(fptr, "Total Time taken: %.2fs\n", total_time_taken);


    fclose(fptr);

    // Free memory used by the PocketSphinx speech recognizer and configuration
    ps_free(ps);
    cmd_ln_free_r(config);
}


/**
* @brief Initializes configuration of the recognizer and enables noise removal, iterates
*        over the audio files to be processed.
*
* @return None.
*/
void With_Remove_Noise() {
    // Initialize the configuration for the PocketSphinx speech recognizer
    config = cmd_ln_init(NULL, ps_args(), TRUE,
        "-hmm", "model/model_parameters/demo.ci_cont",
        "-lm", "model/etc/demo.lm",                      
        //"-kws", "model/keywords.list",
        "-dict", "model/etc/demo.dic",
        "-logfn", "model/log",
        "-cmninit", "56.0",
        "-remove_noise", "yes",
        NULL);


    // Check if the configuration was successfully created
    if (config == NULL) {
        fprintf(stderr, "Failed to create config object, see log for details\n");
        return;
    }

    // Initialize the PocketSphinx speech recognizer using the configuration
    ps = ps_init(config);

    // Check if the recognizer was successfully created
    if (ps == NULL) {
        fprintf(stderr, "Failed to create recognizer, see log for details\n");
        return;
    }

    fptr = fopen("./results/with_remove_noise_results.txt", "w");
    fprintf(fptr, "Remove noise: Yes\n");
    // Loop through all audio files and phrases to recognize speech
    for (int i = 1; i < 26; ++i)
    {
        //speaker 1 to 90 -> contains audio files for jbrf hsmy hmsz (1,2,3)
        for (int j = 1; j < 8; ++j)
        {
            // Create the file path for the current audio file
            audiofilepath = "model/testing_data/speaker_" + std::to_string(i) + "/" + std::to_string(i) + "_0" + std::to_string(j) + ".wav";
            std::cout << "\nAudio file: " << audiofilepath << std::endl;
            fprintf(fptr, "\nAudio file: %s\n", audiofilepath.c_str());
            // Set the correct phrase for the current audio file
            std::string correctPhrase = "";
            if (j == 1) correctPhrase = "JIANBURUFEI";
            if (j == 2) correctPhrase = "HUNSHUIMOYU";
            if (j == 3) correctPhrase = "HUOMAOSANZHANG";
            if (j == 4) correctPhrase = "JINGANGBUHUAI";
            if (j == 5) correctPhrase = "YAQUEWUSHENG";
            if (j == 6) correctPhrase = "ZHIMAKAIMEN";
            if (j == 7) correctPhrase = "GONGXIFACAI";

            // Recognize speech from the current audio file
            recogniseFromFile(audiofilepath, correctPhrase);
        }

    }

    printf("\n\nIn summary (with remove noise):\n");
    fprintf(fptr, "\n\nIn summary (with remove noise):\n");

    // Print number of insertions, substitutions and deletions
    printf("\nInsertions: %d    Substitutions: %d   Deletions: %d\n", num_insertions, num_substitutions, num_deletions);
    fprintf(fptr, "\nInsertions: %d    Substitutions: %d   Deletions: %d\n", num_insertions, num_substitutions, num_deletions);

    // Calculate total WER
    double totalWER = (double)(num_insertions + num_substitutions + num_deletions) / (double)num_audio_files;
    printf("\nTotal Word Error Rate (WER): %.2f%%\n", totalWER * 100.0);
    fprintf(fptr, "\nTotal Word Error Rate (WER): %.2f%%\n", totalWER * 100.0);

    // Print out the number of successful recognitions
    std::cout << "Number of successful recognitions : " << successCounter << "/" << num_audio_files << "\n";
    fprintf(fptr, "Number of successful recognitions : %d/%d\n", successCounter, num_audio_files);

    // Calculate the recognition accuracy
    double accuracy = (double)successCounter / (double)num_audio_files;
    printf("Recognition accuracy: %.2f%%\n", accuracy * 100.0);
    fprintf(fptr, "Recognition accuracy: %.2f%%\n\n", accuracy * 100.0);

    // Print out total time taken
    printf("Total Time taken: %.2fs\n", total_time_taken);
    fprintf(fptr, "Total Time taken: %.2fs\n", total_time_taken);

    fclose(fptr);

    // Free memory used by the PocketSphinx speech recognizer and configuration
    ps_free(ps);
    cmd_ln_free_r(config);
}


/**
* @brief Initializes configuration of the recognizer, enables noise removal and keyword search,
         iterates over the audio files to be processed.
*
* @return None.
*/
void KWS_With_Remove_Noise() {
    // Initialize the configuration for the PocketSphinx speech recognizer
    config = cmd_ln_init(NULL, ps_args(), TRUE,
        "-hmm", "model/model_parameters/demo.ci_cont",
        "-kws", "model/keywords.list",
        "-dict", "model/etc/demo.dic",
        "-logfn", "model/log",
        "-cmninit", "56.0",
        "-remove_noise", "yes",
        NULL);


    // Check if the configuration was successfully created
    if (config == NULL) {
        fprintf(stderr, "Failed to create config object, see log for details\n");
    }

    // Initialize the PocketSphinx speech recognizer using the configuration
    ps = ps_init(config);

    // Check if the recognizer was successfully created
    if (ps == NULL) {
        fprintf(stderr, "Failed to create recognizer, see log for details\n");
    }

    fptr = fopen("./results/kws_with_remove_noise_results.txt", "w");
    fprintf(fptr, "Keyword Search (KWS): Yes\n");
    fprintf(fptr, "Remove noise: Yes\n");
    // Loop through all audio files and phrases to recognize speech
    for (int i = 1; i < 26; ++i)
    {
        //speaker 1 to 90 -> contains audio files for jbrf hsmy hmsz (1,2,3)
        for (int j = 1; j < 8; ++j)
        {
            // Create the file path for the current audio file
            audiofilepath = "model/testing_data/speaker_" + std::to_string(i)    + "/" + std::to_string(i) + "_0" + std::to_string(j) + ".wav";
            std::cout << "\nAudio file: " << audiofilepath << std::endl;
            fprintf(fptr, "\nAudio file: %s\n", audiofilepath.c_str());
            // Set the correct phrase for the current audio file
            std::string correctPhrase = "";
            if (j == 1) correctPhrase = "JIANBURUFEI";
            if (j == 2) correctPhrase = "HUNSHUIMOYU";
            if (j == 3) correctPhrase = "HUOMAOSANZHANG";
            if (j == 4) correctPhrase = "JINGANGBUHUAI";
            if (j == 5) correctPhrase = "YAQUEWUSHENG";
            if (j == 6) correctPhrase = "ZHIMAKAIMEN";
            if (j == 7) correctPhrase = "GONGXIFACAI";

            // Recognize speech from the current audio file
            recogniseFromFile(audiofilepath, correctPhrase);
        }

    }

    printf("\n\nIn summary (with remove noise):\n");
    fprintf(fptr, "\n\nIn summary (with remove noise):\n");

    // Print number of insertions, substitutions and deletions
    printf("\nInsertions: %d    Substitutions: %d   Deletions: %d\n", num_insertions, num_substitutions, num_deletions);
    fprintf(fptr, "\nInsertions: %d    Substitutions: %d   Deletions: %d\n", num_insertions, num_substitutions, num_deletions);

    // Calculate total WER
    double totalWER = (double)(num_insertions + num_substitutions + num_deletions) / (double)num_audio_files;
    printf("\nTotal Word Error Rate (WER): %.2f%%\n", totalWER * 100.0);
    fprintf(fptr, "\nTotal Word Error Rate (WER): %.2f%%\n", totalWER * 100.0);

    // Print out the number of successful recognitions
    std::cout << "Number of successful recognitions : " << successCounter << "/" << num_audio_files << "\n";
    fprintf(fptr, "Number of successful recognitions : %d/%d\n", successCounter, num_audio_files);

    // Calculate the recognition accuracy
    double accuracy = (double)successCounter / (double)num_audio_files;
    printf("Recognition accuracy: %.2f%%\n", accuracy * 100.0);
    fprintf(fptr, "Recognition accuracy: %.2f%%\n\n", accuracy * 100.0);

    // Print out total time taken
    printf("Total Time taken: %.2fs\n", total_time_taken);
    fprintf(fptr, "Total Time taken: %.2fs\n", total_time_taken);

    fclose(fptr);

    // Free memory used by the PocketSphinx speech recognizer and configuration
    ps_free(ps);
    cmd_ln_free_r(config);
}

void resetCounters() {
    num_frames = 0;
    max_amplitude = 0;
    successCounter = 0;
    num_audio_files = 0;
    num_insertions = 0;
    num_substitutions = 0;
    num_deletions = 0;
    total_time_taken = 0.0;
}
int main(int argc, char* argv[])
{
    // Run the 3 tests and reset counters after each test
    Without_Remove_Noise();
    resetCounters();
    With_Remove_Noise();
    resetCounters();
    KWS_With_Remove_Noise();

    return 0;
}