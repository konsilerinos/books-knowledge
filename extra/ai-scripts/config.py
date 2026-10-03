import os

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
TARGET_DIR = os.path.join(SCRIPT_DIR, "../")

LOGO_NAME = "logo.jpg" 

INJECTED_STYLES = """
    <style>
        body { font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Helvetica, Arial, sans-serif; padding: 30px; background: #fafafa; color: #333; line-height: 1.5; margin: 0; }
        .container { max-width: 1000px; margin: 0 auto; background: #fff; padding: 20px; border: 1px solid #e1e4e8; border-radius: 6px; box-shadow: 0 1px 3px rgba(0,0,0,0.05); box-sizing: border-box; }
        .header { display: flex; align-items: center; gap: 15px; margin-bottom: 20px; border-bottom: 1px solid #eaecef; padding-bottom: 15px; }
        .breadcrumbs { font-size: 1.3em; font-weight: 600; }
        .breadcrumbs a { color: #0366d6; text-decoration: none; }
        .breadcrumbs a:hover { text-decoration: underline; }
        .breadcrumbs span { color: #586069; }
        .wrapped-content-container { padding-top: 15px; width: 100%; overflow-x: auto; }
    </style>
"""
