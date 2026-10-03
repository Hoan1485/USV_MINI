import os
import re

def process_file(filepath):
    with open(filepath, 'r', encoding='utf-8') as f:
        content = f.read()

    pattern = re.compile(r'(/\*.*?\*/)|(//.*?$)', re.DOTALL | re.MULTILINE)

    def replacer(match):
        comment = match.group(0)
        
        if "USER CODE BEGIN" in comment or "USER CODE END" in comment:
            return comment
            
        if "---" in comment and "/*" in comment and "*/" in comment:
             return comment
             
        if comment.startswith("/**"):
            return comment
            
        if comment.startswith("// Hàm") or comment.startswith("// Hàm ") or comment.startswith("/* Hàm"):
            return comment
            
        # keep standard CubeMX header comments like /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
        if "Reset of all peripherals" in comment or "Configure the system clock" in comment or "Initialize all configured peripherals" in comment or "Infinite loop" in comment:
            return comment

        return "" 

    new_content = pattern.sub(replacer, content)
    
    # clean up multiple blank lines, but only if they are empty
    new_content = re.sub(r'\n[ \t]*\n[ \t]*\n', '\n\n', new_content)
    
    with open(filepath, 'w', encoding='utf-8') as f:
        f.write(new_content)

print("Testing on main.c")
process_file('Core/Src/main.c')
