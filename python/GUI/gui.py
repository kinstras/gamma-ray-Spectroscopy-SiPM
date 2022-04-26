from tkinter import *
import pandas as pd
from pandas import DataFrame
import time
import tkinter as tk
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import threading
import numpy as np
import logging
import os
import shutil
from matplotlib.animation import FuncAnimation
from tkinter import messagebox
import serial.tools.list_ports

#global variable
pause = False

class Gui:
    
    
    def __init__(self):
        try:
            
            print("Enter your path of your source folder: ")
            self.source_folder_path = input()
            print("Enter your path of your destination folder: ")
            self.destination_folder_path = input()
            self.setting_gui()
        except KeyboardInterrupt:
            print("Keyboard Interruption")
            self.root.destroy
        finally:
            print("Session Closed")
        
    
    
    def setting_gui(self):
        """
        Sets the main Graphical User Interface with tKinder.
        Returns
        -------
        None.
        """
        self.root = Tk()
        self.root.geometry('1400x900')  
        self.root.title("LISER")  
        Label(self.root, text='LISER Project', font='Arial  15').place(x=0, y=0)
        
        ####### Setting GUI #######
        menu = Menu(self.root)
        self.root.config(menu=menu)
        filemenu = Menu(menu)
        menu.add_cascade(label='File', menu=filemenu)
        filemenu.add_command(label='New')
        filemenu.add_command(label='Open...')
        filemenu.add_command(label='Save as')
        filemenu.add_separator()
        filemenu.add_command(label='Exit', command=self.root.quit)
        helpmenu = Menu(menu)
        menu.add_cascade(label='Help', menu=helpmenu)
        helpmenu.add_command(label='About')
        
        var = StringVar()
        label = Message( self.root, anchor = CENTER, textvariable=var, relief=RAISED, width = 100, cursor = "dot" )

        #var is just a mock-up, we will create a "console window" in order to log-print
        #some messages in right-bottom of GUI
        var.set("Hey!? How are you doing?")
        label.place(x=1000, y=700)

        #Setting one figure and four axes that belongs to it
        self.fig, ((self.ax1, self.ax2), (self.ax3, self.ax4)) = plt.subplots(2, 2, figsize=(11, 6))
        self.fig.tight_layout(pad=3)

        self.ax1.set(title='Time- Altitude Plot', ylabel='Altidute [m]', xlabel='Time [sec]')
        self.ax2.set(title='Altitude- Pressure Plot', ylabel='Pressure [bar]', xlabel='Altitude [m]')
        self.ax3.set(title='Smth- Pressure Plot', ylabel='Smth [bar]', xlabel='Altitude [m]')

        #self.ax3 = plt.subplot(223,projection ='3d')
        #self.ax3.set(title='3d line for altitude', ylabel='Height [m]', xlabel='Time [sec]')

        self.ax4 = plt.subplot(224)
        self.ax4.set(title='Gamma ray spectrum ', ylabel='Events / 1.639 keV', xlabel='Energy [keV]')
        #initialize x & y
        x=[]
        y=[]
        #draw the lines
        (self.line1,) = self.ax1.plot(x,y,'r-')
        (self.line2,) = self.ax2.plot(x,y)

                
        self.canvas = FigureCanvasTkAgg(self.fig, master=self.root)
        self.canvas.get_tk_widget().pack(side=TOP)
        
        
        # Set Buttons
        self.button_start = Button(self.root, text='Start', font='Arial  15',width=12, fg='green' , command = self.start_session)
        self.button_stop = Button(self.root, text='Stop', font='Arial  15',width=12, fg='red', command=self.askMe)   #self.root.destroy
        self.button_pause = Button(self.root, text='Pause', font='Arial  15',width=12)
        self.button_pause.bind('<Button-1>', self.onPause)
        self.button_screenshot = Button(self.root, text='Screenshot', font='Arial  15',width=12, command = self.screenshot)
        self.button_continue = Button(self.root, text='Continue', font='Arial  15',width=12, command = self.ccontinue)
        self.button_master1 = Button(self.root, text='Master', font='Arial  15',width=12, command = self.master1)
        self.button_batsave= Button(self.root, text='Battery Save', font='Arial  15',width=12,  command = self.battery_save)
        #Placing Buttons
        self.button_start.place(x=100, y=600)
        self.button_screenshot.place(x=300, y=600)
        self.button_pause.place(x=100, y=650)
        self.button_continue.place(x=300, y=650)
        self.button_master1.place(x=100, y=700)
        self.button_batsave.place(x=300, y=700)
        self.button_stop.place(x=500, y=650)
        
        
        mainloop()

    
    def read_serial_data(self):
        """
        Reads bytes from serial port and returns the package
        """
        
        ports = serial.tools.list_ports.comports()
        serialInst = serial.Serial()

        portList = []

        for onePort in ports:
            portList.append(str(onePort))
            print (str(onePort))

        val = input ("select Port :") 

        for x in range (0,len(portList)):
            if portList[x].startsswith("" + str(val)):
                portVar = ""  + str(val)
                print(portList[x])

        serialInst.baudrate = 9600
        serialInst.port = portVar
        serialInst.open()

        while True:
            if serialInst.in_waiting:
                packet = serialInst.readline()
                print(packet.decode('utf' ).rstrip('\n'))
        return packet
    
    def askMe(self):
        """
        Creates a pop up window when you press the "stop button"
        and needs confirmation to exit
        Returns
        -------
        None.
        """
        reply = messagebox.askquestion('Confirm exit', 'Do you want to exit ?')
        if reply == 'yes':
            self.root.destroy()
        elif reply == 'no':
            pass
        else:
            messagebox.showwarning('error', 'Something went wrong!')
    
    
    
    def start_session(self):
        """
        Creates an object of FuncAnimation class and calls self.plot() every 1 sec
        Returns
        -------
        None.
        """
        packet = self.read_serial_data()
        self.flag = 0   # flag metavliti gia na mpei MONO MIA FORA stin if sinthiki
        #self.anim_running = True
        self.Myanimation = FuncAnimation(self.fig, self.plot, interval=1000,blit=False)
        print(self.Myanimation)
        print("inside start_session")
        plt.tight_layout()
        self.canvas.draw()

    def onPause(self,event):
        """
        Pause reading txts and takes screenshots, need continue button
        So far only stops the animation
        Returns
        -------
        None.
        """
        #pass
        
        print("on Pause")
        self.Myanimation.event_source.stop()

        pass
    def screenshot(self):
        """
        Takes screenshots of plots
        """
        pass
    def ccontinue(self):
        """
        Continue the data acquisition
        """
        self.Myanimation.event_source.start()

    def master1(self):
        """
        Triggers arduino to start spectroscopy or stops the first mission
        """
        pass

    def battery_save(self):
        """
        Closes the unnecessary mission
        """
        pass

    def plot(self, i):
        """
        Reads .txt files from "source_folder" and send them in
        "destination_folder", sets values for x&y , and calls update functions
        for each plot
        Returns
        -------
        None.
        """
        try:
         
            list = sorted(os.listdir(self.source_folder_path) , key = lambda x: int(os.path.splitext(x)[0]))  #sort by number due to lambda function
            print(list[0])              #pairno to proto stoixeio
            source = self.source_folder_path + '/' + list[0]
            
            self.df = pd.read_fwf(source, header=None)      #to txt to metatrepo se dataframe
            self.df_spectrum = pd.read_fwf(source, header=None) 
            self.df.dropna(axis='columns')               #petao ta null values
            self.df_spectrum.dropna(axis='columns')
            # Remove all columns between column index 1 to 3
            
            self.df_spectrum.drop(self.df_spectrum.iloc[:, 0:16], inplace = True, axis = 1)
            
            print(self.df_spectrum)
            #self.df = self.df[[9,10,11]]
            #print("Arxiko dataframe: " ,self.df)
            if self.flag == 0:
                self.df_first_mission = self.df             #to df_sum itan adeio kai to gemizoume me tis times tou df
                self.df_sum = self.df_spectrum
                
                self.flag = 1
                print("mesa stin if",self.df_sum)
            else: 
                self.df_first_mission = self.df_first_mission.append(self.df, ignore_index=True)
                self.df_sum = self.df_sum.add(self.df_spectrum, fill_value=0)
                print("inside else")
            
            #proergasia gia spectrum
            columns = self.df_sum.shape[1]
            array = np.array(self.df_sum)
            array.resize(1,columns) #kanw resize ton array se (1,12)
            #total_counts = np.sum(array) #vrisko sinoliko arithmo counts 
            #print("Total counts: ",total_counts)
            cps_list = array[0].tolist()  #metatrepo to array se lista
            energy_list = np.arange(0, 5, 1)
            #energy_list = np.arange(0, 1401.856, 1.369)   with 1024 channels
            #print(cps_list)

            #move file to destination_folder
            shutil.move(source,self.destination_folder_path)
            
            ############# Setting variables #############
            TEAM_ID = self.df_first_mission[0]
            STATE = self.df_first_mission[1]
            MEASUREMENT_ID = self.df_first_mission[2]
            REPETITION_ID = self.df_first_mission[3]
            MEASUREMENT_TIMESTAMP = self.df_first_mission[4]
            MISSION_TIME = self.df_first_mission[5]
            GPS_TIME = self.df_first_mission[6]
            GPS_LONGITUDE = self.df_first_mission[7]
            GPS_LATITUDE = self.df_first_mission[8]
            ALTITUDE = self.df_first_mission[9]
            PRESSURE = self.df_first_mission[10]
            TEMPERATURE = self.df_first_mission[11]
            ENERGY_PER_CHANNEL = self.df_first_mission[12]
            ACQUITION_TIME = self.df_first_mission[13]
            COUNTS_PER_SECOND = self.df_first_mission[14]

            message = self.infoFunction(TEAM_ID,STATE,MEASUREMENT_ID,REPETITION_ID,MEASUREMENT_TIMESTAMP, MISSION_TIME, GPS_TIME, GPS_LONGITUDE, GPS_LATITUDE,ALTITUDE, PRESSURE, TEMPERATURE, ENERGY_PER_CHANNEL, ACQUITION_TIME, COUNTS_PER_SECOND)
            print(message)
            plt.cla()
            #call plot function
            self.refreshPlot1(MISSION_TIME,ALTITUDE)
            self.refreshPlot2(ALTITUDE,PRESSURE)
            self.refreshPlot3(TEMPERATURE,PRESSURE)
         
            #self.refresh3dPlot(GPS_LONGITUDE,GPS_LATITUDE,ALTITUDE)
            self.refreshSpectrum(energy_list,cps_list)
            plt.tight_layout()
            #plt.show()
            
        except IndexError:
                print("File not found.")
        
        
       
    def infoFunction(self, team_id, state, measurement_id, repetition_id, measurement_timestap, mission_time, gps_time,gps_longitude, gps_latitude, altitude, pressure,temperature, energy_per_channel, acquitition_time, counts_per_channel):
        self.team_id = team_id
        self.state = state
        self.measurement_id = measurement_id
        self.repetition_id = repetition_id
        self.measurement_timestap = measurement_timestap
        self.mission_time = mission_time
        self.gps_time = gps_time
        self.gps_longitude = gps_longitude
        self.gps_latitude = gps_latitude
        self.altitude = altitude
        self.pressure = pressure
        self.temperature = temperature
        self.energy_per_channel = energy_per_channel
        self.acquitition_time = acquitition_time
        self.counts_per_channel = counts_per_channel

        print("###################### INFO MESSAGE ######################")
        return "{Team_ID: " + str(self.team_id) + "\n" + "state: " + str(self.team_id)+ "\n" + "measurement_id: " + str(self.measurement_id)+ "\n" + "repetition_id: " + str(self.repetition_id)+ "\n" + "measurement_timestamp: " + str(self.measurement_timestamp)+ "\n" + "mission_time: " + str(self.mission_time) + "\n" + "gps_time: " + str(self.gps_time)+ "\n" + "gps_longitude: " + str(self.gps_longitude)+ "\n" + "gps_latitude: " + str(self.gps_latitude)+ "\n" + "altitude: " + str(self.altitude)+ "\n" + "pressure: " + str(self.pressure)+ "\n" + "temperature: " + str(self.temperature)+ "\n" + "energy_per_channel: " + str(self.energy_per_channel)+ "\n" + "acquitition_time: " + str(self.acquitition_time)+ "\n" + "counts_per_channel: " + str(self.counts_per_channel)+ "}" 

    
    def refreshPlot1(self,x,y):
        """
        Parses data to line1 and re-arrange axes
        Returns
        -------
        None.
        """
        self.line1.set_data(x,y)
        ax = self.canvas.figure.axes[0]
        ax.set_xlim(0, x.max())
        ax.set_ylim(y.min(), y.max())        
        self.canvas.draw()
        
    def refreshPlot2(self,x,y):
        """
        Parses data to line2 and re-arrange axes
        Returns
        -------
        None.
        """
        self.line2.set_data(x,y)
        ax = self.canvas.figure.axes[1]
        ax.set_xlim(x.min(), x.max())
        ax.set_ylim(y.min(), y.max())        
        self.canvas.draw()
        
    def refresh3dPlot(self,x,y,z):
        """
        Parses data to line3 and plot a 3d model
        Returns
        -------
        None.
        """
        # Setting the axes properties
##        ax.set(xlim3d=(0, 1), xlabel='X')
##        ax.set(ylim3d=(0, 1), ylabel='Y')
##        ax.set(zlim3d=(0, 1), zlabel='Z')
        self.ax3.plot3D(x, y, z)[2]
        self.canvas.draw()

    def refreshSpectrum(self,x,y):
        """
        Parses data to spectrum and plot a histogram that
        represent the gamma-ray spectroscopy
        Returns
        -------
        None.
        """
        self.ax4.bar(x,y, color='blue', width=0.1)
        self.canvas.draw()


obj = Gui()

