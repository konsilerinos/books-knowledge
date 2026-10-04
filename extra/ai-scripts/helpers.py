import os
import subprocess
import shutil
import re
import hashlib

def get_readable_size(size_in_bytes):
    if size_in_bytes >= 1024 * 1024:
        return f"{size_in_bytes / (1024 * 1024):.1f} M"
    elif size_in_bytes >= 1024:
        return f"{size_in_bytes / 1024:.0f} K"
    return f"{size_in_bytes} B"


def get_relative_depth(current_dir, root_dir):
    rel_path = os.path.relpath(current_dir, root_dir)
    if rel_path == ".":
        return ""
    levels = len([p for p in rel_path.split(os.sep) if p])
    return "../" * levels


def generate_breadcrumbs(current_dir, root_dir, is_file=False, file_name=""):
    rel_path = os.path.relpath(current_dir, root_dir)
    back_to_root = get_relative_depth(current_dir, root_dir)
    
    if rel_path == ".":
        if is_file:
            return f'<a href="{back_to_root}index.html">Extra</a> / <span>{file_name}</span>'
        return '<span>Extra</span> / '
        
    html_crumbs = [f'<a href="{back_to_root}index.html">Extra</a> / ']
    parts = [p for p in rel_path.split(os.sep) if p]
    
    for i, part in enumerate(parts):
        if i == len(parts) - 1 and not is_file:
            html_crumbs.append(f'<span>{part}</span> / ')
        else:
            steps_up = len(parts) - 1 - i
            link = "../" * steps_up
            html_crumbs.append(f'<a href="{link}index.html">{part}</a> / ')
            
    if is_file:
        html_crumbs.append(f'<span>{file_name}</span>')
        
    return "".join(html_crumbs)


def minify_html(html_string):
    html_string = re.sub(r'>\s+<', '><', html_string)
    html_string = re.sub(r'\s+', ' ', html_string)
    return html_string.strip()

def compile_typst_to_pdf(file_path, root_dir, current_logo_url):
    output_pdf_path = os.path.splitext(file_path)[0] + ".pdf"
    file_dir = os.path.dirname(os.path.abspath(file_path))
    
    current_check = file_dir
    project_root = file_dir
    while True:
        parent = os.path.dirname(current_check)
        if os.path.basename(current_check) == "extra" or parent == current_check:
            project_root = current_check
            break
        current_check = parent

    cmd = [
        "typst", "compile",
        "--root", project_root,
        file_path,
        output_pdf_path
    ]
    
    try:
        subprocess.run(cmd, check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, encoding="utf-8", cwd=file_dir)
        success = True
    except Exception as e:
        print(f"Ошибка компиляции Typst в PDF: {e}")
        success = False
        
    if success and os.path.exists(output_pdf_path):
        file_crumbs = generate_breadcrumbs(file_dir, root_dir, is_file=True, file_name=os.path.basename(file_path))
        back_to_root = get_relative_depth(file_dir, root_dir)
        
        rel_pdf_url = os.path.basename(output_pdf_path)

        ennobled_html = f"""<!DOCTYPE html>
<html lang="ru">
<head>
    <meta charset="UTF-8">
    <title>{os.path.basename(file_path)}</title>
    <link rel="stylesheet" href="{back_to_root}styles.css">
    <style>
        .pdf-viewer {{
            width: 100%;
            height: calc(100vh - 101px); 
            border: 1px solid #e1e4e8;
            box-sizing: border-box;
            display: block;
        }}
    </style>
</head>
<body>
<div class="container">
    <div class="header">
        <img src="{current_logo_url}" alt="Logo" width="48" height="48" onerror="this.style.display='none'">
        <div class="breadcrumbs">
            {file_crumbs}
        </div>
    </div>
    <div class="wrapped-content-container">
        <iframe id="pdfPlayer" class="pdf-viewer"></iframe>
    </div>
</div>

<script>
    function updatePdfSource() {{
        const iframe = document.getElementById('pdfPlayer');
        const hash = window.location.hash;
        
        if (hash) {{
            const targetLabel = hash.replace('#', '');
            iframe.src = "{rel_pdf_url}#nameddest=" + encodeURIComponent(targetLabel) + "&toolbar=0&navpanes=0";
        }} else {{
            iframe.src = "{rel_pdf_url}#toolbar=0&navpanes=0";
        }}
    }}

    window.addEventListener('DOMContentLoaded', updatePdfSource);
    window.addEventListener('hashchange', updatePdfSource);
</script>
</body>
</html>"""

        output_html_path = os.path.splitext(file_path)[0] + ".html"
        with open(output_html_path, "w", encoding="utf-8") as f:
            f.write(ennobled_html)

    return success
