import pandas as pd
import numpy as np
from scipy.fft import fft
from scipy.signal import spectrogram
import matplotlib.pyplot as plt
from random import randrange
from scipy.signal import butter, filtfilt


    

def highpass_filter(data, cutoff_freq, fs):
    nyquist_freq = 0.5 * fs
    normal_cutoff = cutoff_freq / nyquist_freq
    b, a = butter(2, normal_cutoff, btype='high', analog=False)
    filtered_data = filtfilt(b, a, data, axis=0)
    return filtered_data


# Load data from CSV files

def load_data(file_path):
    data = np.loadtxt(file_path, delimiter=",")
    accel_data = data[:, :3]
    gyro_data = data[:, 3:]
    return accel_data, gyro_data


fs = 0.5  # Sampling frequency
nperseg = 8  # Length of each segment
noverlap = nperseg // 2  # Overlap between segments

def plot_fft(data, file):
    filtered_data = highpass_filter(data, 0.2, fs)

    fft_x = np.fft.fft(filtered_data[:, 0])
    fft_y = np.fft.fft(filtered_data[:, 1])
    fft_z = np.fft.fft(filtered_data[:, 2])

    
    # Compute frequencies
    n = len(data)
    frequencies = np.fft.fftfreq(n, d=1)

    # Plot combined FFT for all axes
    plt.figure(figsize=(10, 8))
    plt.plot(frequencies, fft_x, "r-", label="X-axis")
    plt.plot(frequencies, fft_y, "g-", label="Y-axis")
    plt.plot(frequencies, fft_z, "b-", label="Z-axis")
    plt.title(f"FFT of ({file})")
    plt.xlabel("Frequency (Hz)")
    plt.ylabel("Amplitude")
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.savefig(f"{file}_fft.png")

def plot_spectrogram(data,filename):
    # Compute magnitude of combined accelerometer data
    data_magnitude = np.sqrt(np.sum(data**2, axis=1))
    
    # Compute spectrogram for combined accelerometer magnitude
    f_axis, t_axis, Sxx = spectrogram(data_magnitude, fs=fs, nperseg=nperseg, noverlap=noverlap)
    
    # Plot spectrogram for combined accelerometer magnitude
    plt.figure(figsize=(10, 8))
    plt.pcolormesh(t_axis, f_axis, 10 * np.log10(Sxx), shading='gouraud', cmap='viridis')
    plt.title(f'Spectrogram of Combined Accelerometer Magnitude of {filename}')
    plt.ylabel('Frequency [Hz]')
    plt.xlabel('Time [sec]')
    plt.colorbar(label='Power/Frequency (dB/Hz)')
    plt.tight_layout()
    plt.savefig(f'{filename}_magnitude_spectrogram.png')  # Save the plot

for direction in ["up", "down", "left", "right"]:
    file_1 = randrange(0,20)
    file_2 = randrange(0,20)
    while file_1 == file_2:
        file_1 = randrange(0,20)
    file_1 = file_1 if file_1 >= 10 else f'0{file_1}'
    file_2 = file_2 if file_2 >= 10 else f'0{file_2}'
    
    accel, gyro = load_data(f"{direction}/{direction}_{file_1}")
    accel_2, gyro_2 = load_data(f"{direction}/{direction}_{file_2}")
    
    plot_fft(accel, f'accel_{direction}_{file_1}')
    plot_fft(accel_2, f'accel_{direction}_{file_2}')
    plot_fft(gyro, f'gyro_{direction}_{file_1}')
    plot_fft(gyro_2, f'gyro_{direction}_{file_2}')

    plot_spectrogram(accel, f'accel_{direction}_{file_1}')
    plot_spectrogram(accel_2, f'accel_{direction}_{file_2}')
    plot_spectrogram(gyro, f'gyro_{direction}_{file_1}')
    plot_spectrogram(gyro_2, f'gyro_{direction}_{file_2}')
    