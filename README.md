# Entry List Editor

A simple CPP that edits your Assetto Corsa entry lists based on the names of your cars to add AI=fixed or AI=none


# ❗ Disclaimer
Code looks for the words traffic in to find traffic cars, if your not using traffic cars in the name, this wont work.
It also doesn't have the functionality to set AI to auto yet. In the future, possibly add on to make the code more advanced. 

Written on `Windows` environment


# ⚙️ Configuration

~~In the editor.py, put in the input file location and output file location (need to be folders~~
```
EX; INPUT_FOLDER = r"C:\Users\Desktop\entry_list editor\Entry list editor\input"
    OUTPUT_FOLDER = r"C:\Users\Desktop\entry_list editor\Entry list editor\output"
```
In VS Code
- Open "Developer Command Prompt for VS" (search it in Start menu)
- Navigate to your .cpp folder: ``cd C:\path\to\folder``
- Run: ``cl /EHsc /std:c++17 entry_list_fixer.cpp /Fe:entry_list_fixer.exe``

Normally, once you have the exe, all you should need to do is drag the ini or txt file you want into the exe, and it will output the new file and keep the old one. 
