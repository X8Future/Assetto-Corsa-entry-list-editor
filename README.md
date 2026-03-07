# Entry List Editor

A simple CPP that edits your Assetto Corsa entry lists based on the names of your cars to add AI=fixed or AI=none


# ❗ Disclaimer
Code looks for the words traffic in to find traffic cars, if your not using traffic cars in the name, this wont work.
It also doesn't have the functionality to set AI to auto yet. In the future, possibly add on to make the code more advanced. 

Written on `Windows` environment


# ⚙️ Edit and compile your own
## In Google
- Go to https://code.visualstudio.com/docs/cpp/config-mingw and download the direct installer

- Run the exe and click through the installation

- Once installed search on your computer ``MYSYS2`` and open it

- Once you open it paste in this code ``pacman -S --needed base-devel mingw-w64-ucrt-x86_64-toolchain``

- Click ``Enter`` and ``Enter`` agian, then press ``Y`` and ``Enter``

- Once its finished installing go to ``Environment Variables`` and go to ``Environment Variables`` , edit the ``PATH`` section

-  Click ``New`` and put ``C:\msys64\ucrt64\bin``

## In VS Code
- Open "Extensions" section

- Look up ``C/C++`` and install it

- Now At the top click the run button then press ``Run C/C++ File``

Normally, it will compile the exe and you should be good to go

# ❗ Regular Use
If your looking to just use the exe, download the exe from the versions and it just drag and drop your entry_list.ini's. If your looking to edit the code follow the instructions above
