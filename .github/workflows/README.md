# GitHub Actions Workflows

## Build and Deploy to GitHub Pages

This workflow automatically builds the Linux/Wasm project and deploys it to GitHub Pages.

### Workflow Overview

**File:** `build-and-deploy.yml`

**Triggers:**
- Push to `main` or `master` branch
- Pull requests to `main` or `master` branch
- Manual trigger via workflow dispatch

**Jobs:**

1. **build** - Builds the entire project
   - Installs Clang 15, LLVM, and dependencies
   - Builds Linux kernel (→ `kernel.wasm`)
   - Builds BusyBox (→ `busybox.wasm`)
   - Builds all graphics examples
   - Creates initramfs (→ `initramfs.cpio`)
   - Prepares deployment directory with runtime files
   - Creates beautiful landing page

2. **deploy** - Deploys to GitHub Pages
   - Uses official GitHub Pages actions
   - Deploys the `_site` directory
   - Provides deployment URL in summary

### Build Artifacts

The workflow produces:
- `kernel.wasm` - Linux kernel compiled to WebAssembly
- `initramfs.cpio` - Initial RAM filesystem with BusyBox
- `example-graphics.wasm` - Basic graphics demo
- `example-shaders.wasm` - Shader compilation demo
- `example-texture.wasm` - Textured quad demo
- `example-cube.wasm` - 3D spinning cube
- `example-demo.wasm` - 7-cube showcase (recommended!)

### Deployment

The workflow deploys:
- All runtime files (`runtime/*`)
- Compiled kernel and initramfs
- All graphics examples
- Custom landing page with project showcase

### Setting Up GitHub Pages

To enable automatic deployment:

1. **Go to Repository Settings**
   - Navigate to `Settings` → `Pages`

2. **Configure Source**
   - Source: `GitHub Actions`
   - (No need to select a branch when using Actions)

3. **Enable Permissions** (if needed)
   - Go to `Settings` → `Actions` → `General`
   - Under "Workflow permissions":
     - Select "Read and write permissions"
     - Check "Allow GitHub Actions to create and approve pull requests"
   - Save changes

4. **Trigger the Workflow**
   - Push to main branch, or
   - Go to `Actions` tab → `Build and Deploy to GitHub Pages` → `Run workflow`

5. **Access Your Site**
   - After successful deployment, your site will be available at:
   - `https://kyroskoh.github.io/linux-wasm/`

### Build Summary

After each build, GitHub Actions provides a summary with:
- ✅ Build status
- 📦 Artifact sizes
- 🎨 Examples built
- 🚀 Deployment status
- 🔗 Live URL

### Local Testing

To test the build process locally:

```bash
# Build everything (same as CI)
./linux-wasm.sh build-kernel
./linux-wasm.sh build-busybox
./linux-wasm.sh build-graphics-examples
./linux-wasm.sh create-initramfs

# Verify artifacts
ls -lh kernel.wasm initramfs.cpio

# Test locally
cd runtime
python3 -m http.server 8000
# Visit http://localhost:8000
```

### Troubleshooting

**Build Fails:**
- Check that all submodules are properly initialized
- Ensure Clang 15 is available
- Check build logs in Actions tab

**Deployment Fails:**
- Verify GitHub Pages is enabled in repository settings
- Check that workflow has write permissions
- Ensure `GITHUB_TOKEN` has proper permissions

**Site Not Loading:**
- Clear browser cache
- Check browser console for errors
- Verify all files were deployed correctly

### Performance

**Build Time:** ~3-5 minutes
- Kernel: ~1 minute
- BusyBox: ~1-2 minutes
- Graphics Examples: ~30 seconds
- Initramfs: ~10 seconds

**Deployment Time:** ~30 seconds

**Total:** ~5-6 minutes from push to live

### Customization

**Change the landing page:**
Edit the HTML in the "Prepare deployment directory" step of `build-and-deploy.yml`

**Add more examples:**
Add build commands in the "Build graphics examples" step

**Modify deployment:**
Adjust the files copied in the "Prepare deployment directory" step

### Resources

- [GitHub Pages Documentation](https://docs.github.com/en/pages)
- [GitHub Actions Documentation](https://docs.github.com/en/actions)
- [deploy-pages Action](https://github.com/actions/deploy-pages)
- [upload-pages-artifact Action](https://github.com/actions/upload-pages-artifact)

