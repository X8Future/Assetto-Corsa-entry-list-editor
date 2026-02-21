import os
import re
import time
from datetime import datetime

INPUT_FOLDER = r"C:\Users\adria\Desktop\entry_list editor\Entry list editor\input"
OUTPUT_FOLDER = r"C:\Users\adria\Desktop\entry_list editor\Entry list editor\output"

os.makedirs(OUTPUT_FOLDER, exist_ok=True)

def process_file(input_path):
    print(f"Starting to process: {os.path.basename(input_path)}")
    try:
        with open(input_path, "r", encoding="utf-8") as f:
            content = f.read()

        sections = re.split(r"(?=\[CAR_\d+\])", content)
        new_sections = []

        for section in sections:
            if not section.strip():
                continue

            section = re.sub(r"\nAI=.*", "", section)  # remove old AI lines
            ai_line = "AI=none" if "/ADAn" in section else "AI=fixed"
            section = re.sub(r"(RESTRICTOR=0)(\s*)", r"\1\n" + ai_line, section, count=1)
            new_sections.append(section.strip())

        def car_number(section):
            match = re.match(r"\[CAR_(\d+)\]", section)
            return int(match.group(1)) if match else 0

        new_sections.sort(key=car_number)

        edited = "\n\n".join(new_sections) + "\n"

        # create unique output file using timestamp
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        output_file = os.path.join(OUTPUT_FOLDER, f"entry_list_{timestamp}.ini")

        with open(output_file, "w", encoding="utf-8") as f:
            f.write(edited)

        print(f"Finished editing: {os.path.basename(input_path)} -> {os.path.basename(output_file)}")

        os.remove(input_path)
        print(f"Deleted input file: {os.path.basename(input_path)}")

    except Exception as e:
        print(f"⚠️ Issue editing {os.path.basename(input_path)}: {e}")

processed = {}

print("✅ Bot started. Watching input folder...")

while True:
    for filename in os.listdir(INPUT_FOLDER):
        input_path = os.path.join(INPUT_FOLDER, filename)
        if not os.path.isfile(input_path):
            continue

        mtime = os.path.getmtime(input_path)

        if filename not in processed or processed[filename] != mtime:
            process_file(input_path)
            processed[filename] = mtime

    time.sleep(5)
