# Quick Start Guide

## What Changed?

Your background removal tool has been refactored to fix the color cast problem:

**OLD:** Used RGB color detection (`is_skin_tone`) → Failed with red/yellow tinted photos  
**NEW:** Uses saturation detection (`is_likely_subject`) → Works regardless of color cast

## Files Overview

### New Files (Must Add)
- `include/detect.h` - Subject detection API
- `src/detect.c` - Saturation-based detection implementation

### Modified Files (Must Replace)
- `include/config.h` - Added `SUBJECT_SATURATION_THRESHOLD`
- `src/process.c` - Now uses `detect.h` instead of embedded `is_skin_tone()`

### Unchanged Files
- `src/main.c`
- `src/queue.c`
- `include/queue.h`
- `include/process.h`
- All STB headers

## Installation

### Option 1: Using Makefile (Recommended)

```bash
# 1. Replace your project files with the new ones
# 2. Build using the provided Makefile
make

# 3. Run the program
./whitebg photo.jpg
```

### Option 2: Manual GCC Compilation

**Linux/Mac:**
```bash
gcc -o whitebg \
    src/main.c \
    src/process.c \
    src/queue.c \
    src/detect.c \
    src/stb_lib.c \
    -I./include -lm -O2
```

**Windows (MinGW):**
```bash
gcc -o whitebg.exe \
    src/main.c \
    src/process.c \
    src/queue.c \
    src/detect.c \
    src/stb_lib.c \
    -I./include -lm -O2 resources.o
```

### Option 3: Update Existing Makefile

Add `src/detect.c` to your SOURCES list:

```makefile
SOURCES = src/main.c src/process.c src/queue.c src/detect.c src/stb_lib.c
```

## Usage (Same as Before)

```bash
# Basic usage (uses defaults)
./whitebg photo.jpg

# Custom threshold and quality
./whitebg photo.jpg 85 95
```

**Output:** Creates `white_T80_Q90_photo.jpg` in the same directory

## Tuning for Your Photos

If the algorithm is removing too much or too little, adjust the saturation threshold:

**Edit `include/config.h`:**

```c
// More aggressive (removes more, might clip subject edges)
#define SUBJECT_SATURATION_THRESHOLD 22

// Default (balanced)
#define SUBJECT_SATURATION_THRESHOLD 18

// More conservative (protects more, might leave background artifacts)
#define SUBJECT_SATURATION_THRESHOLD 14
```

Then recompile:
```bash
make clean
make
```

## Testing Your Changes

### Test Case 1: Normal Photo
```bash
./whitebg normal_photo.jpg
```
Expected: Works as before

### Test Case 2: Photo with Red/Yellow Tint
```bash
./whitebg scanned_photo.jpg
```
Expected: Now works correctly (this was broken before)

### Test Case 3: Very Dark Background
```bash
./whitebg dark_bg_photo.jpg 60 90
```
Expected: Might need lower threshold (try 50-70)

## Troubleshooting

### Problem: Background is still protected (not removed)

**Solution:** Increase saturation threshold
```c
// In config.h, change from 18 to 22 or 25
#define SUBJECT_SATURATION_THRESHOLD 22
```

### Problem: Subject edges (hair, chin) are being removed

**Solution:** Decrease saturation threshold
```c
// In config.h, change from 18 to 14 or 15
#define SUBJECT_SATURATION_THRESHOLD 14
```

### Problem: Compilation error "detect.h not found"

**Solution:** Ensure `-I./include` flag is set and files are in correct directories:
```
project/
├── include/detect.h  ← Must be here
└── src/detect.c      ← Must be here
```

### Problem: Linker error "undefined reference to is_likely_subject"

**Solution:** Ensure `src/detect.c` is being compiled:
```bash
# Add to your GCC command:
src/detect.c
```

## Understanding the Algorithm

### How Saturation Detection Works

**Saturation** = `max(R,G,B) - min(R,G,B)`

| Pixel Type | RGB | Saturation | Protected? |
|------------|-----|------------|-----------|
| Pure white | (255,255,255) | 0 | No |
| White w/ tint | (255,245,240) | 15 | No |
| Beige background | (240,235,230) | 10 | No |
| Human skin | (220,180,160) | 60 | Yes |
| Clothing | (180,100,90) | 90 | Yes |

**Key insight:** Backgrounds have low RGB variance (low saturation), while subjects have high RGB variance (high saturation), regardless of color cast.

## Project Structure

```
project/
├── include/
│   ├── config.h          ← Settings (NEW threshold added)
│   ├── detect.h          ← NEW detection API
│   ├── process.h         ← Unchanged
│   ├── queue.h           ← Unchanged
│   ├── stb_image.h       ← STB library
│   └── stb_image_write.h ← STB library
├── src/
│   ├── detect.c          ← NEW saturation detection
│   ├── main.c            ← Unchanged
│   ├── process.c         ← UPDATED (uses detect.h)
│   ├── queue.c           ← Unchanged
│   └── stb_lib.c         ← Unchanged
├── Makefile              ← Build automation
└── resources.o           ← Windows icon (optional)
```

## Next Steps

1. ✅ Build the project with the new files
2. ✅ Test with your existing photos
3. ✅ Test with color-cast photos (previously broken)
4. ⚙️ Adjust `SUBJECT_SATURATION_THRESHOLD` if needed
5. 📖 Read `REFACTORING_GUIDE.md` for detailed explanation
6. 📊 Read `CODE_COMPARISON.md` to see before/after code

## Getting Help

- **Build errors:** Check that all files are in correct directories
- **Runtime issues:** Adjust `SUBJECT_SATURATION_THRESHOLD` in `config.h`
- **Understanding changes:** Read `REFACTORING_GUIDE.md`
- **Code comparison:** See `CODE_COMPARISON.md`

## Summary

🎯 **Problem Solved:** Color cast no longer breaks subject detection  
🏗️ **Architecture Improved:** Detection logic is now modular and testable  
⚙️ **Configuration Added:** Saturation threshold is now adjustable  
🔄 **Backward Compatible:** Same CLI interface, same output format  
📚 **Well Documented:** Comprehensive guides included  

**You can now process phone-scanned passport photos with color cast!**
