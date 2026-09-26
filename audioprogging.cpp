#include<iostream>
#include<fstream>
#include<cmath>

using namespace std;
//header stuff
//Riff

const string chunk_id = "RIFF";
const string chunk_size = "----";
const string format = "WAVE";

//FMT 

const string subchunk1_id = "fmt ";
const int subchunk1_size = 16;
const int audio_format = 1;
const int num_channels = 2;
const int sample_rate = 44100;
const int byte_rate = sample_rate * num_channels * (subchunk1_size / 8);
const int block_align = num_channels * (subchunk1_size / 8);
const int bits_per_sample = 16;

//data

const string subchunk2_id = "data";
const string subchunk2_size = "----";

const int duration = 20;
const int max_amplitude = 32760;

void write_as_bytes(ofstream& file, int value, int byte_size)
{
	file.write(reinterpret_cast<const char*>(&value), byte_size);
}

int main()
{
	ofstream wav;
	wav.open("test.wav", ios::binary);
	
	if (wav.is_open())
	{
		wav << chunk_id;
		wav << chunk_size;
		wav << format; 

		wav << subchunk1_id;
		write_as_bytes(wav, subchunk1_size, 4);
		write_as_bytes(wav, audio_format, 2);
		write_as_bytes(wav, num_channels, 2);
		write_as_bytes(wav, sample_rate, 4);
		write_as_bytes(wav, byte_rate, 4);
		write_as_bytes(wav, block_align, 2);
		write_as_bytes(wav, bits_per_sample, 2);

		wav << subchunk2_id;
		wav << subchunk2_size;
		
		int start_audio = wav.tellp();
		int prec = 1000;

		int a4 = 0;
		int g3 = 0;
		int g2 = 0;
		int d2 = 0;

		for (int i = 0; i < sample_rate * duration; i++) // 882000 is the max here;
		{
			//double amp = (double)i / sample_rate * max_amplitude;


			double vale = 0;
			//A4 note
			if ((i >= 0 && i <= 26460) || (i >= 132300 && i <= 167580) || (i >= 171990 && i <= 198400)
				|| (i >= 198450 && i <= 211580) || (i >= 211680 && i <= 238140) || (i >= 238140 && i <= 319020)
				|| (i >= 343980 && i <= 380670) || (i >= 383670 && i <= 410130))
				//1 beat == 26460
			{
				vale = vale + sin((2 * 3.14 * a4 * 440) / sample_rate);
				a4++;
			}
			else if (a4 > 0) a4 = 0;
			//G3 note
			if ((i > 26460 && i <= 86790) || (i > 92610 && i <= 119000) || (i> 119070 && i<= 145530))
			{
				vale = vale + sin((2 * 3.14 * g3 * 391.1) / sample_rate);

			}
			else if (g3 > 0) g3 = 0;

			//G2 note
			if ((i >= 0 && i<= 88200) || (i>= 92610 && i<= 105830) || (i>= 105840 && i<= 171940) //here, either the 2nd or the first node ends too early/ starts too latee
			|| (i>= 171990 && i<= 185220) || (i>=198450 && i<= 211660)) {
				vale = vale + sin((2 * 3.14 * g2 * 391.1/2) / sample_rate);
			}
			else if (g2 > 0) g2 = 0;
			//D2 note
			if ((i >= 211680 && i <= 264450) || (i >= 264600 && i <= 277730) || (i >= 277830 && i <= 357150) 
			|| (i >= 357210 && i<= 410100) || (i >= 410130 && i <= 423360)) {
				vale = vale + sin((2 * 3.14 * d2 * 293.66 / 2) / sample_rate);
			}
			else if (d2> 0) d2 = 0;
			





			// metronome 
			if (i % (26460) == 0 || prec<1000) 
			{
				if (prec == 0) { prec = 1000; }
				else prec--;

				vale = vale + (sin((2 * 3.14 * i * 220 * 5) / sample_rate)/2 + (sin((2 * 3.14 * i * 220*6) / sample_rate))/3 + (sin((2 * 3.14 * i * 220*7) / sample_rate)))/4;

			}

			//For the drums I am planning on making opening the wav of the hi hat, snare, 808
			//Then I add them to the track, basically, upon the already existing sin values for the ch1 and ch2 i'll add the binary 
			//Of the channel 1 anad channel 2 of the wav file,,
			//incrementing 4 bytes, until the audio file ends, after which i start from the start
			

			//To keep within amplitude I'd have to scale it dodwn a bit i guess

			//Mostly have to focus on implementing the actuall notes
			//Then have to add attack delay release thingys, probably using e^-x decaying functions for them


			//channels
			double ch1 = max_amplitude/3 * vale;
			//double ch2 = (max_amplitude - amp) * vale;
			double ch2 = max_amplitude / 3 * vale;


			//writing channels
			write_as_bytes(wav, ch1, 2);
			write_as_bytes(wav, ch2, 2);
		}
		int end_audio = wav.tellp();
		wav.seekp(start_audio - 4);
		write_as_bytes(wav, end_audio - start_audio, 4);

		wav.seekp(4, ios::beg);
		write_as_bytes(wav, end_audio - 8, 4);
	}
	wav.close();
	return 0;
}