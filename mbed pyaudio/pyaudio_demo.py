import pyaudio
import wave
import time
import serial
from functools import reduce

# open wave file
wf = wave.open("test1.wav", 'rb')
# instantiate PyAudio
audio = pyaudio.PyAudio()
# instantiate Serial
ser = serial.Serial('com3', 115200)


# define callback
def callback(in_data, frame_count, time_info, status):
    data = wf.readframes(frame_count)
    return data, pyaudio.paContinue


# change audio speed rate (*bug : chipmunk effect)
def change_speed(rate):
    stream_buffer = audio.open(format=audio.get_format_from_width(wf.getsampwidth()),
                               channels=wf.getnchannels(),
                               rate=int(wf.getframerate() * rate),
                               output=True,
                               stream_callback=callback)
    stream.stop_stream()
    stream_buffer.start_stream()
    return stream_buffer


def str2float(s):
    def fn(x, y):
        return x * 10 + y

    n = s.index('.')
    s1 = list(map(int, [x for x in s[:n]]))
    s2 = list(map(int, [x for x in s[n + 1:]]))
    return reduce(fn, s1) + reduce(fn, s2) / 10 ** len(s2)


# open stream using callback
stream = audio.open(format=audio.get_format_from_width(wf.getsampwidth()),
                    channels=wf.getnchannels(),
                    rate=int(wf.getframerate()),
                    output=True,
                    stream_callback=callback)

# start the stream

stream.stop_stream()
print("play")
while True:
    time.sleep(1)
    data = ser.readline()
    print(data)
    if data == b'start\n':
        print(data)
        stream.start_stream()
        break
    else:
        time.sleep(1)

##########################################################
# main section to control speed rate
while stream.is_active():
    data = ser.readline()
    if data == b'end\n':
        break
    data_str = data.decode()
    read_rate = str2float(data_str.strip()) / 100
    if read_rate > 0.5:
        stream = change_speed(read_rate)
    else:
        stream = change_speed(0.5)
    print(str2float(data_str.strip()) / 100)
    # time.sleep(0.1)
##########################################################

# stop stream
print("finished")
stream.stop_stream()
stream.close()
wf.close()

# close PyAudio
audio.terminate()
