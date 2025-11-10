# GitHub Pages Setup Guide

This guide will help you set up GitHub Pages for your Linux/Wasm repository, enabling automatic deployment with every push to the main branch.

## Quick Start

### 1. Enable GitHub Pages

1. Go to your repository on GitHub
2. Click **Settings** (tab at the top)
3. Scroll down and click **Pages** (in the left sidebar under "Code and automation")
4. Under "Build and deployment":
   - **Source**: Select `GitHub Actions`
   - (You don't need to select a branch when using Actions)
5. Click **Save**

### 2. Configure Workflow Permissions

1. Go to **Settings** → **Actions** → **General** (in the left sidebar)
2. Scroll to "Workflow permissions"
3. Select **"Read and write permissions"**
4. ✅ Check **"Allow GitHub Actions to create and approve pull requests"**
5. Click **Save**

### 3. Trigger the Workflow

The workflow will automatically run when you:
- Push to the `main` or `master` branch
- Create a pull request to `main` or `master`

Or you can manually trigger it:
1. Go to the **Actions** tab
2. Click on **"Build and Deploy to GitHub Pages"**
3. Click **"Run workflow"** button
4. Select branch (usually `main`)
5. Click **"Run workflow"**

### 4. Wait for Deployment

- The build process takes ~5-6 minutes
- Watch progress in the **Actions** tab
- When complete, a green ✅ will appear

### 5. Access Your Live Site

Your site will be available at:
```
https://<your-username>.github.io/<repository-name>/
```

For example:
- Username: `joelseverin`
- Repository: `linux-wasm`
- URL: `https://joelseverin.github.io/linux-wasm/`

## What Gets Deployed

The workflow automatically deploys:

### Core System
- `kernel.wasm` - Linux kernel (~15 MB)
- `initramfs.cpio` - Initial RAM filesystem with BusyBox (~5 MB)
- Runtime files (HTML, JavaScript, CSS)

### Graphics Examples
- `example-graphics.wasm` - Basic color animation
- `example-shaders.wasm` - Colored triangle with shaders
- `example-texture.wasm` - Textured quad demo
- `example-cube.wasm` - Spinning 3D cube
- `example-demo.wasm` - **7-cube showcase (recommended!)**

### Landing Page
A beautiful landing page is automatically created with:
- Project overview
- Feature highlights
- Quick stats
- Links to GitHub repository
- Direct launch button

## Customization

### Change the Landing Page

Edit the HTML in `.github/workflows/build-and-deploy.yml`:

1. Find the "Prepare deployment directory" step
2. Locate the `cat > _site/index.html << 'EOF'` section
3. Modify the HTML between `EOF` markers
4. Commit and push

### Update Your Repository URL

If your repository name or username is different, update the link in `README.md`:

```markdown
**🚀 Try it live:** [https://YOUR-USERNAME.github.io/YOUR-REPO/](https://YOUR-USERNAME.github.io/YOUR-REPO/)
```

### Add More Examples

To include additional graphics examples:

1. Create your C program (e.g., `runtime/examples/my-demo.c`)
2. Add build command to `linux-wasm.sh`:
   ```bash
   echo "Compiling my-demo.c..."
   clang -target wasm32-unknown-unknown -nostdlib ... -o my-demo.wasm my-demo.c
   cp my-demo.wasm busybox/bin/
   ```
3. Commit and push - GitHub Actions will build and deploy it!

## Troubleshooting

### Build Fails

**Error: Clang not found**
- The workflow installs Clang 15 automatically
- If it fails, check the build logs in Actions tab

**Error: Submodule issues**
- Ensure all submodules are committed
- Run locally: `git submodule update --init --recursive`

**Error: Build script fails**
- Test locally first: `./linux-wasm.sh build-kernel`
- Check that all patches apply cleanly
- Review error logs in Actions tab

### Deployment Fails

**Error: "Workflow does not have write permissions"**
- Go to Settings → Actions → General
- Select "Read and write permissions"
- Try again

**Error: "Pages deployment failed"**
- Ensure GitHub Pages is enabled (Settings → Pages)
- Check that source is set to "GitHub Actions"
- Verify `GITHUB_TOKEN` permissions

**Error: "No pages artifact found"**
- The build job must complete successfully first
- Check build logs for errors
- Ensure `actions/upload-pages-artifact@v3` runs

### Site Doesn't Load

**Blank Page**
- Check browser console (F12) for errors
- Verify all files deployed (check Actions → Deploy job logs)
- Try hard refresh (Ctrl+Shift+R or Cmd+Shift+R)

**404 Errors**
- Ensure repository name matches URL
- Check that `.nojekyll` file exists in root
- Verify Pages source is set to "GitHub Actions"

**WebAssembly Errors**
- Ensure CORS headers are correct (GitHub Pages handles this)
- Check that `kernel.wasm` and `initramfs.cpio` are present
- Open browser console to see specific errors

### Performance Issues

**Slow Loading**
- First load downloads ~20 MB (kernel + initramfs + examples)
- Subsequent loads use browser cache
- Consider hosting on CDN for better performance

**Build Takes Too Long**
- Normal build time: 5-6 minutes
- Kernel build: ~1-2 minutes
- BusyBox: ~1-2 minutes
- Examples: ~30 seconds
- If significantly longer, check Actions logs for bottlenecks

## Advanced Configuration

### Custom Domain

To use a custom domain (e.g., `linux-wasm.example.com`):

1. Add `CNAME` file to repository root:
   ```
   linux-wasm.example.com
   ```
2. Configure DNS:
   - Add CNAME record pointing to `<username>.github.io`
3. GitHub Pages → Settings → Custom domain
4. Enter your domain and save

### Environment Variables

Customize the build by setting environment variables in the workflow:

```yaml
env:
  LW_DEBUG_CFLAGS: "-g3"  # Debug info level
  LW_WORKSPACE: "/custom/path"  # Workspace directory
```

### Build Matrix

To test multiple configurations:

```yaml
strategy:
  matrix:
    debug: ["-g3", ""]
    
steps:
  - name: Build with debug=${{ matrix.debug }}
    env:
      LW_DEBUG_CFLAGS: ${{ matrix.debug }}
    run: ./linux-wasm.sh build-kernel
```

## Monitoring

### View Deployment Status

1. Go to **Actions** tab
2. Click on latest workflow run
3. See build and deployment progress
4. Check summary for:
   - ✅ Build status
   - 📦 Artifact sizes
   - 🎨 Examples built
   - 🔗 Live URL

### Check Deployment History

1. Go to **Settings** → **Pages**
2. See "Latest deployment" section
3. View all deployments and their status

### Monitor Usage

GitHub Pages has usage limits:
- **Bandwidth**: 100 GB/month
- **Build time**: 10 minutes per build
- **Storage**: 1 GB

Check usage in:
- Settings → Pages → View usage

## Security

### HTTPS

- GitHub Pages automatically provides HTTPS
- All connections are encrypted
- No configuration needed

### Content Security

- SharedArrayBuffer requires secure context (HTTPS)
- GitHub Pages provides this automatically
- Local testing requires HTTPS or special headers

### Secrets

Don't commit sensitive data:
- Use GitHub Secrets for API keys
- Never hardcode credentials
- Review all files before pushing

## Resources

- [GitHub Pages Documentation](https://docs.github.com/en/pages)
- [GitHub Actions Documentation](https://docs.github.com/en/actions)
- [Linux/Wasm Documentation](runtime/GRAPHICS.md)
- [Troubleshooting Guide](.github/workflows/README.md)

## Support

If you encounter issues:

1. Check this guide first
2. Review `.github/workflows/README.md`
3. Check Actions logs for errors
4. Open an issue with:
   - Error message
   - Build logs
   - Steps to reproduce

## Success!

Once deployed, you can share your Linux/Wasm instance with anyone! They can:
- Try it directly in their browser
- No installation required
- Works on any device with a modern browser
- Full 3D graphics support
- Interactive shell with BusyBox utilities

Share your URL:
```
🚀 Try Linux in your browser:
https://<your-username>.github.io/<repository-name>/
```

Enjoy! 🎉

