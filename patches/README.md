# PhantomFrame VLC Patches

This directory contains patches for integrating PhantomFrame watermarking into VLC Media Player.

## Overview

The patches modify VLC's x264 encoder to support imperceptible video watermarking using Quantization Parameter (QP) adjustments on 8x8 blocks.

## Files

- `vlc_x264_watermark.patch` - Main patch for x264 encoder integration

## Prerequisites

- VLC Media Player source code (version 3.0 or later)
- x264 library development headers
- Patch utility (patch, git apply, etc.)

## Applying the Patch

### Method 1: Using patch utility

```bash
cd /path/to/vlc/source
patch -p1 < /path/to/phantomframe/patches/vlc_x264_watermark.patch
```

### Method 2: Using git apply

```bash
cd /path/to/vlc/source
git apply /path/to/phantomframe/patches/vlc_x264_watermark.patch
```

### Method 3: Manual application

If automatic patching fails, manually apply the changes shown in the patch file to the corresponding VLC source files.

## What the Patch Does

1. **Adds watermark configuration variables** to the x264 encoder
2. **Integrates watermark application** during the encoding process
3. **Provides user controls** for enabling/disabling watermarking
4. **Configures watermark parameters** like payload, seed, and block density

## Configuration Options

After applying the patch, the following VLC options become available:

- `--x264-watermark-enabled` - Enable/disable watermarking
- `--x264-watermark-payload` - 16-byte watermark payload (hex format)
- `--x264-watermark-seed` - Random seed for block selection
- `--x264-block-density` - Fraction of blocks to watermark (0.0-1.0)

## Example Usage

```bash
# Enable watermarking with custom payload
vlc input.mp4 --sout="#transcode{vcodec=h264,venc=x264{profile=baseline,level=3.1,watermark-enabled=1,watermark-payload=0123456789abcdef,watermark-seed=12345,block-density=0.3}}" --sout-keep

# Disable watermarking (default)
vlc input.mp4 --sout="#transcode{vcodec=h264,venc=x264{profile=baseline,level=3.1}}" --sout-keep
```

## Building VLC with Watermarking

After applying the patch:

```bash
# Configure VLC build
./bootstrap
./configure --enable-x264

# Build VLC
make -j$(nproc)

# Install (optional)
sudo make install
```

## Troubleshooting

### Patch fails to apply
- Ensure you have the correct VLC version
- Check if the source files have been modified
- Try applying the patch manually

### Compilation errors
- Verify x264 development headers are installed
- Check that all dependencies are satisfied
- Ensure the patch was applied correctly

### Runtime errors
- Verify the watermark library is properly linked
- Check VLC configuration options
- Review VLC logs for error messages

## Integration Notes

- The patch requires the PhantomFrame watermark library to be built and linked
- Watermarking is disabled by default to maintain compatibility
- Performance impact is minimal when watermarking is disabled
- The patch maintains backward compatibility with existing VLC installations

## Support

For issues with the VLC patches:
1. Check the PhantomFrame main documentation
2. Review VLC build logs
3. Verify patch application
4. Ensure all dependencies are met

## License

The patches are provided under the same license as PhantomFrame (see main LICENSE file).
