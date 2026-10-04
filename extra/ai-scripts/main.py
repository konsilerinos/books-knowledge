import os
import datetime
from config import SCRIPT_DIR, TARGET_DIR, LOGO_NAME
from helpers import get_readable_size, generate_breadcrumbs, compile_typst_to_pdf, get_relative_depth, minify_html
import subprocess

def build_index_for_dir(root_dir, current_dir):
    try:
        items = os.listdir(current_dir)
    except Exception as e:
        print(f"Ошибка чтения папки {current_dir}: {e}")
        return

    display_path = os.path.relpath(current_dir, root_dir)
    if display_path == ".":
        current_logo_url = LOGO_NAME
    else:
        levels = len([p for p in display_path.split(os.sep) if p])
        current_logo_url = "../" * levels + LOGO_NAME

    for item in items:
        if item.lower().endswith(".typ") and not item.startswith("."):
            full_typ_path = os.path.join(current_dir, item)
            print(f"Компиляция Typst в PDF: {os.path.relpath(full_typ_path, root_dir)}...")
            compile_typst_to_pdf(full_typ_path, root_dir, current_logo_url) 

    try:
        items = os.listdir(current_dir)
    except Exception:
        return

    sub_dirs = []
    html_files = []

    for item in items:
        if (item.startswith(".") or 
            item == "index.html" or 
            item.lower() == LOGO_NAME.lower() or
            item.lower().endswith(".typ") or
            item.lower().endswith(".pdf")): # Пропускаем и .pdf, чтобы они не двоились в таблице с .html
            continue

        full_path = os.path.join(current_dir, item)

        if os.path.isdir(full_path):
            has_html = any(
                f.lower().endswith(".html") or f.lower().endswith(".typ")
                for r, d, files in os.walk(full_path) for f in files
            )
            if has_html:
                sub_dirs.append(item)
        elif os.path.isfile(full_path) and item.lower().endswith(".html"):
            html_files.append(item)

    sub_dirs.sort()
    html_files.sort()

    title_path = "/" if display_path == "." else f"/{display_path.replace(os.sep, '/')}"
    back_to_root = get_relative_depth(current_dir, root_dir)

    html_content = f"""<!DOCTYPE html>
<html lang="ru">
<head>
    <meta charset="UTF-8">
    <title>Index of {title_path}</title>
    <link rel="stylesheet" href="{back_to_root}styles.css">
</head>
<body>
<svg xmlns="http://w3.org" style="display: none;">
    <g id="icon-link">
        <path d="M4.715 6.542 3.343 7.914a3 3 0 1 0 4.243 4.243l1.828-1.829A3 3 0 0 0 8.586 5.5L8 6.086a1 1 0 0 0-.154.199 2 2 0 0 1 .861 3.337L6.88 11.45a2 2 0 1 1-2.83-2.83l.793-.792a4 4 0 0 1-.128-1.287z"/>
        <path d="M6.586 4.672A3 3 0 0 0 7.414 9.5l.586-.586a1 1 0 0 0 .154-.199 2 2 0 0 1-.861-3.337L9.12 3.12a2 2 0 1 1 2.83 2.83l-.793.792a4 4 0 0 1 .128 1.287l1.372-1.372a3 3 0 1 0-4.243-4.243z"/>
    </g>
</svg>

<div class="container">
    <div class="header">
        <img src="{current_logo_url}" alt="Logo" width="48" height="48" onerror="this.style.display='none'">
        <div class="breadcrumbs">
            {generate_breadcrumbs(current_dir, root_dir, is_file=False)}
        </div>
    </div>
    <table>
        <tbody>
"""

    if os.path.abspath(current_dir) != os.path.abspath(root_dir):
        html_content += """        <tr>
            <td><a href="../index.html">&#128194; ../</a></td>
            <td class="meta-col">-</td>
            <td class="size-col">-</td>
        </tr>\n"""

    for d in sub_dirs:
        full_path = os.path.join(current_dir, d)
        try:
            git_log = subprocess.run(
                ["git", "log", "-1", "--format=%cd", "--date=format:%d-%b-%Y %H:%M", full_path],
                capture_output=True, text=True, check=True
            )
            mtime = git_log.stdout.strip()
            if not mtime:
                mtime = datetime.datetime.fromtimestamp(os.stat(full_path).st_mtime).strftime("%d-%b-%Y %H:%M")
        except Exception:
            mtime = datetime.datetime.fromtimestamp(os.stat(full_path).st_mtime).strftime("%d-%b-%Y %H:%M")
        html_content += f"""        <tr>
            <td><a href="{d}/index.html">&#128194; {d}</a></td>
            <td class="meta-col">{mtime}</td>
            <td class="size-col">-</td>
        </tr>\n"""

    for f in html_files:
        full_path = os.path.join(current_dir, f)
        stat = os.stat(full_path)
        size = get_readable_size(stat.st_size)
        
        try:
            git_log = subprocess.run(
                ["git", "log", "-1", "--format=%cd", "--date=format:%d-%b-%Y %H:%M", full_path],
                capture_output=True, text=True, check=True
            )
            mtime = git_log.stdout.strip()
            if not mtime:
                mtime = datetime.datetime.fromtimestamp(stat.st_mtime).strftime("%d-%b-%Y %H:%M")
        except Exception:
            mtime = datetime.datetime.fromtimestamp(stat.st_mtime).strftime("%d-%b-%Y %H:%M")
        
        html_content += f"""        <tr>
            <td><a href="{f}">📄 {f}</a></td>
            <td class="meta-col">{mtime}</td>
            <td class="size-col">{size}</td>
        </tr>\n"""

    html_content += """        </tbody>
    </table>
    
    <div class="footer-links">
        <a href="https://github.com" target="_blank" class="footer-link gh-hover">
            <svg width="14" height="14" fill="currentColor"><use href="#icon-link"/></svg>
            GitHub (todo)
        </a>
        <a href="https://t.me" target="_blank" class="footer-link tg-hover">
            <svg width="14" height="14" fill="currentColor"><use href="#icon-link"/></svg>
            Telegram-chat (todo)
        </a>
    </div>
</div>
</body>
</html>"""

    with open(os.path.join(current_dir, "index.html"), "w", encoding="utf-8") as index_file:
        index_file.write(minify_html(html_content))

    for d in sub_dirs:
        build_index_for_dir(root_dir, os.path.join(current_dir, d))


if __name__ == "__main__":
    if os.path.exists(TARGET_DIR):
        print(f"Скрипт запущен из: {SCRIPT_DIR}")
        print(f"Старт генерации HTML из Typst и обновление оберток в: {TARGET_DIR}...")
        build_index_for_dir(TARGET_DIR, TARGET_DIR)
        print("Вся структура индексов успешно обновлена!")
    else:
        print(f"Ошибка: Папка 'extra' не найдена по пути: {TARGET_DIR}")
    
    input("\nНажмите Enter, чтобы закрыть программу...")