# White Background Maker 📸

A lightweight, high-performance C application designed to automatically convert image backgrounds to white. Specifically optimized for **Passport Size Photographs** and ID cards.

## 🚀 Why This Exists
Most background removal tools are heavy, require internet access, or use complex AI models that are overkill for simple tasks.

I built this tool to solve those problems:
* **Fast:** Runs instantly on your CPU using optimized algorithms.
* **Offline:** No API calls, no cloud uploads, no subscription fees.
* **Integrated:** Works directly from the **Windows Right-Click Menu** for a seamless workflow.
* **Specific:** Optimized for passport photos where the subject is centered and the background is solid (blue/green/white).

## 🛠️ Tech Stack
* **Language:** C (Standard C99)
* **Libraries:** `stb_image` & `stb_image_write` (Single-header libraries for image processing)
* **Algorithm:** Custom **Flood Fill (Breadth-First Search)** for smart edge detection and background removal.
* **Integration:** Windows Registry (`.reg`) for Context Menu integration.

# White Background Maker 📸

A lightweight, high-performance C application designed to automatically convert image backgrounds to white. Specifically optimized for **Passport Size Photographs** and ID cards.

## 🚀 Why This Exists
Most background removal tools are heavy, require internet access, or use complex AI models that are overkill for simple tasks.

I built this tool to solve those problems:
* **Fast:** Runs instantly on your CPU using optimized algorithms.
* **Offline:** No API calls, no cloud uploads, no subscription fees.
* **Integrated:** Works directly from the **Windows Right-Click Menu** for a seamless workflow.
* **Specific:** Optimized for passport photos where the subject is centered and the background is solid (blue/green/white).

## 🛠️ Tech Stack
* **Language:** C (Standard C99)
* **Libraries:** `stb_image` & `stb_image_write` (Single-header libraries for image processing)
* **Algorithm:** Custom **Flood Fill (Breadth-First Search)** for smart edge detection and background removal.
* **Integration:** Windows Registry (`.reg`) for Context Menu integration.
### Part 3: Usage Instructions (Windows & CLI)
## 💻 How to Use

### 🪟 Method 1: Windows Right-Click Menu (Recommended)
Add this tool to your system menu to process images in one click.

1.  Open the `install_menu.reg` file in a text editor.
2.  **Crucial:** Edit the paths inside the file to point to your location (e.g., Change `C:\\Apps\\WhiteBG\\whitebg.exe` to your actual path).
3.  Save and **Double-Click** the `.reg` file to install.

**Workflow:**
1.  Right-click any image (`.jpg`, `.png`).
2.  Select **White Background Maker**.
3.  Choose your mode:
    * **Default:** Standard sensitivity (Good for most photos).
    * **Gentle:** Low sensitivity (Use for light-colored clothes).
    * **Aggressive:** High sensitivity (Use for dark shadows/bad lighting).
    * **Custom:** Manually type the Threshold and Quality.

### 🐧 Method 2: Command Line (Linux / macOS / Windows CMD)
You can run the tool manually from a terminal.

**Syntax:**
```bash
./whitebg <image_path> [threshold] [quality]

# 1. Standard run (Uses config.h defaults)
./whitebg photo.jpg

# 2. Custom Threshold (e.g., 50 for lighter processing)
./whitebg photo.jpg 50

# 3. Custom Threshold & Quality (e.g., Threshold 100, Quality 50%)
./whitebg photo.jpg 100 50
```

### Part 4: Configuration & Structure


## ⚙️ Configuration (config.h)
You can permanently tune the algorithm by editing `include/config.h` and recompiling.

| Macro | Default | Description |
| :--- | :--- | :--- |
| `COLOR_THRESHOLD` | `80.0` | **Sensitivity.** Lower (30) preserves white clothes. Higher (100) removes shadows. |
| `JPEG_QUALITY` | `90` | **Compression.** 1 (Low) to 100 (High). |
| `OUTPUT_PREFIX` | `"white_"` | **Naming.** Prefix added to the new file (e.g., `white_photo.jpg`). |
| `LOGO_PATH` | `"logo.png"` | **Watermark.** Filename of the logo to overlay. |
| `LOGO_OPACITY` | `1.0` | **Transparency.** 0.0 (Invisible) to 1.0 (Solid). |
| `LOGO_PADDING` | `20` | **Position.** Pixels from the bottom-right corner. |

## 📂 Project Structure
```text
WhiteBgMaker/
├── src/
│   ├── main.c        # Entry point, argument parsing, & file saving
│   ├── process.c     # Flood Fill algorithm & Logo blending logic
│   ├── queue.c       # Custom Queue implementation for Flood Fill
│   └── stb_lib.c     # Library implementation wrapper
├── include/
│   ├── config.h      # Central settings file
│   ├── process.h     # Function prototypes
│   ├── queue.h       # Data structure definitions
│   └── stb_...       # Image processing libraries
├── install_menu.reg  # Windows Registry script for context menu
├── .gitignore        # Git ignore rules
└── README.md         # Documentation
