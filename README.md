# What is a Tilling Window Manager?
Window managers determine where applications appear on your screen. Window managers handle tasks like: moving a window from one side of a screen to another, minimizing applications, chose which application is in focus, showing your desktop, and more. Almost every computer has a window manager.

A tilling window manager forces all applications to exist on a grid (e.g. full screen, half screen, quarter screen...). Most of what you do is already on a grid: very few people use applications that are not in full or split screen. If your applications are not overlapping, you are in a grid format. By forcing windows onto a grid, you can vastly improve keybindings and, in turn, increase productivity.

# Demo
![TWM_DEMO](https://github.com/user-attachments/assets/e4f691c4-dede-475c-a2a5-7557fad279e9)

# Key Bindings
<kbd>Mod</kbd> = <kbd>Windows</kbd> by default

<kbd>Mod</kbd> +<kbd>Enter</kbd> | open new terminal

<kbd>Mod</kbd> + <kbd>Shift</kbd> + <kbd>q</kbd> | close current window

<kbd>Mod</kbd> +<kbd>0</kbd> ... <kbd>Mod</kbd>+<kbd>9</kbd> | swap to workstation 0-9 respectively 

<kbd>Mod</kbd> +<kbd>Shift</kbd>+ <kbd>0</kbd> ... <kbd>Mod</kbd>+<kbd>Shift</kbd>+<kbd>9</kbd> | send current window to workstation 0-9 respectively 

<kbd>Mod</kbd> + <kbd>h</kbd> | change focus one window to the left

<kbd>Mod</kbd> + <kbd>j</kbd> | change focus one window up 

<kbd>Mod</kbd> + <kbd>k</kbd> | change focus one window down

<kbd>Mod</kbd> + <kbd>l</kbd> | change focus one window to the right

<kbd>Mod</kbd> + <kbd>h</kbd>+<kbd>shift</kbd> | swap cur window with one window to the left

<kbd>Mod</kbd> + <kbd>j</kbd>+<kbd>shift</kbd> | swap cur window with one window up

<kbd>Mod</kbd> + <kbd>k</kbd>+<kbd>shift</kbd> | swap cur window with one window down 

<kbd>Mod</kbd> + <kbd>l</kbd>+<kbd>shift</kbd> | swap cur window with one window to the right

# Running
Just calling ./twm will not work. There can only be one window manager running at a time and you likely already have one going. You, in turn, have two options: use a virtual machine (recommended) or set this as your current window manager.
(Note: this is for linux only)
#### Virtual Machine: 
I advise getting Xephyr: it allows you to run a second window manager in a toy display. Unlike normal virtual machines, Xephyr runs in the same environment: you  still can access all of your files and interact with programs running on your machine. After getting Xephyr you should install xterm. You can then call ./run.sh to launch the Xephyr environment. 

#### Swap Out Your Window Manager: 
If you want to use this as your actual window manager I would advise looking into dex for Ubuntu / Arch. Essentially just bind ./twm to run on start up. Window managers are fairly integral pieces of software so, it is not a good idea to use one that you don't trust (e.g. mine).  

