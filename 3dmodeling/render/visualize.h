#pragma once
const char* cmd = "ffmpeg -r 60 -f rawvideo -pix_fmt rgba -s 1200x1200 -i - -threads 0 -preset fast -y -pix_fmt yuv420p -crf 28 -vf vflip output.mp4";
unsigned char* image = (unsigned char*)malloc(sizeof(unsigned char) * SCR_WIDTH * SCR_HEIGHT * 4);
// open pipe to ffmpeg's stdin in binary write mode
FILE* ffmpeg = _popen(cmd, "wb");