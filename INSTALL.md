# Installation Guide - Local AI Coding Environment

Step-by-step guide to install everything from scratch on macOS (Apple Silicon).

**Time required:** ~45 minutes (mostly waiting for downloads)

---

## ✅ **Prerequisites**

- **macOS** with Apple Silicon (M1/M2/M3/M4)
- **16+ GB RAM** (36 GB recommended for best experience)
- **50+ GB free disk space** (for models)
- **Internet connection** (for initial downloads)

---

## 📦 **Installation Steps**

### Step 1: Install Ollama (5 minutes)

Ollama is the local LLM runtime that runs your AI models.

```bash
# Download and install Ollama
curl -fsSL https://ollama.com/install.sh | sh

# Verify installation
~/bin/ollama --version
# Should show: ollama version is 0.32.1 or newer

# Start Ollama (it usually starts automatically)
open -a Ollama

# Test it's working
~/bin/ollama list
# Should show an empty list (no models yet)
```

**What this does:**
- Installs Ollama to `/Applications/Ollama.app`
- Creates `~/bin/ollama` command-line tool
- Starts Ollama server on `http://localhost:11434`

---

### Step 2: Install OpenCode (2 minutes)

OpenCode is the AI coding agent that uses the models.

```bash
# Install OpenCode via Homebrew
brew install opencode

# Verify installation
opencode --version
# Should show: 1.18.0 or newer

# Check where it's installed
which opencode
# Should show: /opt/homebrew/bin/opencode (Apple Silicon)
# or /usr/local/bin/opencode (Intel Mac)
```

**Alternative: Install via npm**

If you prefer npm or need the latest version:

```bash
# Install OpenCode globally via npm
npm install -g opencode

# Verify installation
opencode --version

# If not in PATH, add it:
echo 'export PATH="$HOME/.opencode/bin:$PATH"' >> ~/.zshrc
source ~/.zshrc
```

---

### Step 3: Download Models (20-30 minutes)

Download the AI models you'll use. This is the longest step due to file sizes.

#### Download Devstral 24B (Quality Model)

```bash
# Download Devstral Small 2 (15 GB, ~10-15 minutes)
~/bin/ollama pull devstral-small-2:24b

# Create custom model with 32K context
cat > ~/Modelfile.devstral-32k <<'EOF'
FROM devstral-small-2:24b
PARAMETER num_ctx 32768
PARAMETER temperature 0.7
PARAMETER top_p 0.9
PARAMETER repeat_penalty 1.1
EOF

~/bin/ollama create devstral-32k -f ~/Modelfile.devstral-32k

# Test it
~/bin/ollama run devstral-32k "Write a Python function to add two numbers. Just code."
```

#### Download Bonsai 27B (Fast Model)

```bash
# Download Bonsai (3.8 GB, ~3-5 minutes)
~/bin/ollama pull defyma85/bonsai-27b-q1_0

# Create custom model with 32K context
cat > ~/Modelfile.bonsai-32k <<'EOF'
FROM defyma85/bonsai-27b-q1_0
PARAMETER num_ctx 32768
PARAMETER temperature 0.7
PARAMETER top_p 0.9
PARAMETER repeat_penalty 1.1
EOF

~/bin/ollama create bonsai-32k -f ~/Modelfile.bonsai-32k

# Test it
~/bin/ollama run bonsai-32k "Say hello in one word"
```

#### Optional: Download Qwen 3B (Small Model)

```bash
# Download Qwen 3B (2 GB, ~2-3 minutes)
~/bin/ollama pull qwen2.5-coder:3b

# Create custom model
cat > ~/Modelfile.qwen-admin-32k <<'EOF'
FROM qwen2.5-coder:3b
PARAMETER num_ctx 32768
PARAMETER temperature 0.7
EOF

~/bin/ollama create qwen-admin-32k -f ~/Modelfile.qwen-admin-32k
```

**Verify all models:**

```bash
~/bin/ollama list
# Should show:
# - devstral-32k
# - devstral-small-2:24b
# - bonsai-32k
# - defyma85/bonsai-27b-q1_0
# - qwen-admin-32k (if you installed it)
# - qwen2.5-coder:3b (if you installed it)
```

---

### Step 4: Configure OpenCode (5 minutes)

Set up OpenCode to use your local models with profile switching.

#### Create Profile Configs

```bash
# Create OpenCode config directory (if it doesn't exist)
mkdir -p ~/.config/opencode

# Create Fast profile (Bonsai)
cat > ~/.config/opencode/opencode.fast.json <<'EOF'
{
  "$schema": "https://opencode.ai/config.json",
  "provider": {
    "ollama": {
      "npm": "@ai-sdk/openai-compatible",
      "options": {
        "baseURL": "http://localhost:11434/v1"
      },
      "models": {
        "bonsai-32k": {
          "contextWindow": 32768
        }
      }
    }
  },
  "model": "ollama/bonsai-32k",
  "share": "disabled",
  "autoupdate": false
}
EOF

# Create Quality profile (Devstral)
cat > ~/.config/opencode/opencode.quality.json <<'EOF'
{
  "$schema": "https://opencode.ai/config.json",
  "provider": {
    "ollama": {
      "npm": "@ai-sdk/openai-compatible",
      "options": {
        "baseURL": "http://localhost:11434/v1"
      },
      "models": {
        "devstral-32k": {
          "contextWindow": 32768
        }
      }
    }
  },
  "model": "ollama/devstral-32k",
  "share": "disabled",
  "autoupdate": false
}
EOF

# Set Quality as default
cp ~/.config/opencode/opencode.quality.json ~/.config/opencode/opencode.json
```

#### Add Switcher Aliases

```bash
# Add to ~/.zshrc
cat >> ~/.zshrc <<'EOF'

# OpenCode model switchers
alias oc-fast='cp ~/.config/opencode/opencode.fast.json ~/.config/opencode/opencode.json && echo "✓ Fast mode: Bonsai 27B (100-120 tok/s)"'
alias oc-quality='cp ~/.config/opencode/opencode.quality.json ~/.config/opencode/opencode.json && echo "✓ Quality mode: Devstral 24B (75-90 tok/s)"'
alias oc-which='echo "Current model:" && grep -A1 "\"model\":" ~/.config/opencode/opencode.json | tail -1'
EOF

# Reload shell
source ~/.zshrc
```

---

### Step 5: Test Everything (5 minutes)

Verify your complete setup works.

#### Test Ollama

```bash
# Check Ollama is running
~/bin/ollama ps
# Should show no models loaded yet (or recently used models)

# Quick test
~/bin/ollama run devstral-32k "Write hello world in Python. Just code."
# Should return Python code quickly
```

#### Test OpenCode

```bash
# Create a test directory
mkdir -p ~/test-opencode
cd ~/test-opencode

# Create a simple Python file
cat > test.py <<'EOF'
def add(a, b):
    return a + b

print(add(2, 3))
EOF

# Start OpenCode with quality model
oc-quality
opencode

# In OpenCode, try:
# "Add docstrings to this file"
# Type /exit when done
```

#### Test Model Switching

```bash
# Switch to fast model
oc-fast

# Verify switch
oc-which
# Should show: "model": "ollama/bonsai-32k"

# Switch to quality model
oc-quality

# Verify switch
oc-which
# Should show: "model": "ollama/devstral-32k"
```

---

## ✅ **Installation Complete!**

You now have:

- ✅ **Ollama 0.32.1** - Local LLM runtime (MLX optimized)
- ✅ **OpenCode** - AI coding agent
- ✅ **Devstral 24B** - Best quality coding model (15 GB)
- ✅ **Bonsai 27B** - Fast exploration model (3.8 GB)
- ✅ **Profile switching** - Easy model selection with `oc-fast` and `oc-quality`
- ✅ **100% private** - Everything runs locally

---

## 🎯 **Next Steps**

### Learn to Use Your Setup

```bash
# Read the quick start
cat ~/projects/local-inference/README.md

# See all available commands
cat ~/projects/local-inference/OLLAMA_GUIDE.md

# Understand model switching
cat ~/projects/local-inference/OPENCODE_REALITY.md
```

### Start Coding

```bash
# For exploration (fast)
oc-fast
cd ~/your-project
opencode

# For code generation (quality)
oc-quality
cd ~/your-project
opencode
```

---

## 🐛 **Troubleshooting**

### "ollama: command not found"

```bash
# Ollama installs to ~/bin/ollama
# Use full path:
~/bin/ollama --version

# Or add to PATH:
echo 'export PATH="$HOME/bin:$PATH"' >> ~/.zshrc
source ~/.zshrc
```

### "opencode: command not found"

```bash
# If installed via Homebrew, check:
which opencode
# Should be in /opt/homebrew/bin/opencode

# If missing, reinstall:
brew install opencode

# If installed via npm, use full path:
~/.opencode/bin/opencode --version

# Or add to PATH:
echo 'export PATH="$HOME/.opencode/bin:$PATH"' >> ~/.zshrc
source ~/.zshrc
```

### "Connection refused" when using OpenCode

```bash
# Ollama server not running
# Start it:
open -a Ollama

# Or command line:
~/bin/ollama serve
```

### Models downloading very slowly

```bash
# Check internet connection
# Models are large files:
# - Devstral: 15 GB
# - Bonsai: 3.8 GB
# - Qwen 3B: 2 GB

# On slow connection, download one at a time
# You can use the system with just Devstral initially
```

### "Not enough memory" errors

```bash
# Check available RAM
# Minimum requirements:
# - Bonsai: 4 GB RAM
# - Devstral: 16 GB RAM
# - Both: 20 GB RAM

# Use only one model at a time
# Ollama automatically unloads unused models
```

### OpenCode seems stuck or slow

```bash
# Check which model is loaded
~/bin/ollama ps

# Restart OpenCode
# Inside OpenCode: /exit
# Then start again: opencode

# Check if Ollama is responsive
~/bin/ollama run devstral-32k "Hello"
```

---

## 🔧 **Optional: Advanced Setup**

### Custom Context Windows

```bash
# Edit your Modelfile to change context size
cat > ~/Modelfile.devstral-64k <<'EOF'
FROM devstral-small-2:24b
PARAMETER num_ctx 65536  # 64K context
PARAMETER temperature 0.7
EOF

~/bin/ollama create devstral-64k -f ~/Modelfile.devstral-64k
```

### Multiple OpenCode Configs

```bash
# Create project-specific configs
mkdir -p ~/my-project/.opencode

cat > ~/my-project/.opencode/opencode.json <<'EOF'
{
  "$schema": "https://opencode.ai/config.json",
  "provider": {
    "ollama": {
      "npm": "@ai-sdk/openai-compatible",
      "options": { "baseURL": "http://localhost:11434/v1" },
      "models": { "devstral-32k": { "contextWindow": 32768 } }
    }
  },
  "model": "ollama/devstral-32k"
}
EOF

# OpenCode will use this config when run from ~/my-project/
```

### Auto-start Ollama on Login

```bash
# Ollama app already does this
# Check System Settings > General > Login Items
# "Ollama" should be listed
```

---

## 📊 **Disk Space Usage**

After complete installation:

```
~/bin/ollama                     ~200 MB
~/.ollama/models/
  ├── devstral-small-2:24b      ~15 GB
  ├── devstral-32k              ~15 GB (shares layers)
  ├── defyma85/bonsai-27b-q1_0  ~3.8 GB
  ├── bonsai-32k                ~3.8 GB (shares layers)
  └── qwen2.5-coder:3b          ~2 GB (optional)

~/.opencode/                     ~100 MB

Total: ~22-24 GB
```

**Note:** Custom models (devstral-32k, bonsai-32k) share layers with base models, so they don't double the disk usage.

---

## 🔐 **Privacy Verification**

Your setup is 100% local and private:

```bash
# Nothing runs until you start it
# No automatic cloud connections
# All inference happens on your Mac

# Verify no network calls (optional)
# Install Little Snitch or use Activity Monitor
# You'll see NO traffic from Ollama or OpenCode
```

---

## 📚 **Documentation Reference**

After installation, read these guides:

1. **README.md** - Quick start and common workflows
2. **OLLAMA_GUIDE.md** - All Ollama commands
3. **OPENCODE_REALITY.md** - Model switching deep dive
4. **BONSAI_SETUP.md** - Understanding Bonsai model
5. **OPENCODE_PRIVACY.md** - Privacy analysis
6. **SETUP_COMPLETE.md** - Complete system overview

---

## 💡 **Quick Reference Card**

```bash
# Check what's running
~/bin/ollama ps

# List models
~/bin/ollama list

# Switch to fast model
oc-fast

# Switch to quality model
oc-quality

# Check current model
oc-which

# Start OpenCode
opencode

# Test a model directly
~/bin/ollama run devstral-32k "Your question"
```

---

**Installation Guide Version:** 1.0  
**Last Updated:** July 22, 2026  
**Platform:** macOS Apple Silicon (M1/M2/M3/M4)  
**Total Time:** ~45 minutes (mostly downloads)
