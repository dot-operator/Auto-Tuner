# Auto-Tuner
Autotune clone based on US Patent 5973252A. Compiles to audio plugins/VST through the JUCE library.

<<<<<<< HEAD
## Building the plugin ##
This project uses JUCE; you can build it using JUCE's launcher (downloaded from their website [here](https://shop.juce.com/get-juce) ).
A Visual Studio project has also been included.

## Code at a Glance ##
The actual work below is called mostly in PluginProcessor.cpp from ::processBlock, where an input audio buffer is analyzed, and the corrected signal is written back to it.

The pitch detection is done via autocorrelation; an array representing the (not normalized) autocorrelation function and another containing the autocorrelation at zero lag (to compare with in absense of normalization) are updated for each input sample in ::UpdateAutocorrelation.
The actual detected pitch is determined once every five samples (or every sample if the previous attempt failed) in ::TryUpdatePitch by comparing the autocorrelations at the various candidate period lengths with those at zero lag.

Finally, if the pitch is to be corrected, the signal is resampled at the rate needed to change the pitch. Eventually, this would result in a buffer under/overrun, so the output cursor occasionally skips exactly one waveform period length to keep up without introducing clicks or other artifacts.
=======
## Building the project ##
This audio plugin is build with a library called JUCE; you can build it by opening AutoTuner.jucer with JUCE's launcher (downloaded from their website [here](https://shop.juce.com/get-juce) ).
A Visual Studio solution has also been included.
>>>>>>> 3be4e912fbaa7bed0ea1913bce1e5fcce6995b5e
