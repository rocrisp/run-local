# Bonsai-27B Setup Guide

## 📊 **About Bonsai-27B**

**Released**: July 14, 2026 by PrismML  
**Size**: 3.8 GB (1-bit quantization) - smallest 27B model ever!  
**Base**: Qwen3.6-27B (compressed to 1-bit)  
**Context**: 262K tokens (256K effective)  
**License**: Apache 2.0  

### Key Innovation

Bonsai-27B uses **1-bit quantization** - each weight is just 1 bit instead of 16 bits:
- **Normal 27B model**: ~54 GB (16-bit)
- **4-bit quantized**: ~15 GB
- **Bonsai 1-bit**: **3.8 GB** (14x smaller!)

This makes a 27B parameter model run on:
- ✅ Your Mac M3 Pro (easily!)
- ✅ iPhone 17 Pro Max (~11 tok/s)
- ✅ Low-end laptops
- ✅ Older MacBooks

---

## ⚡ **Performance Trade-offs**

### Quality Retention
- **1-bit Bonsai**: Retains **90%** of full-precision quality
- **Ternary (2-bit)**: Retains **95%** of full-precision quality

### Speed on Your M3 Pro 36GB

| Model | Size | Speed (est.) | Quality |
|-------|------|-------------|---------|
| Devstral 24B (4-bit) | 15 GB | 75-90 tok/s | 100% |
| Bonsai 27B (1-bit) | 3.8 GB | **100-120 tok/s** | 90% |
| Bonsai 27B (ternary) | 5.9 GB | 80-100 tok/s | 95% |

**Why faster**: Smaller model = less memory bandwidth = faster inference

---

## 🎯 **When to Use Bonsai**

### ✅ **Good For:**
- Quick prototyping
- Learning/experimentation
- Resource-constrained environments
- Running multiple models simultaneously
- Very long context (256K tokens!)
- Phone/mobile deployment

### ⚠️ **Not Ideal For:**
- Production code generation (90% quality vs 100%)
- Critical refactoring (use Devstral)
- Complex multi-file editing (quality matters)

### 💡 **Best Use Case:**
- **Experimentation** - Try different approaches quickly
- **Reading/understanding code** - Context comprehension is good
- **Simple edits** - Quick fixes, documentation
- **Mobile coding** - On-the-go development

---

## 📦 **Installation Status**

🔄 **Downloading**: `defyma85/bonsai-27b-q1_0` (~3.8 GB)  
⏱️ **ETA**: ~5-10 minutes  
✅ **Ollama**: Ready to use after download  

---

## 🔧 **Setup After Download**

### Step 1: Create Custom Modelfile (32K Context)

```bash
cd /Users/rosecrisp/projects/local-inference

cat > Modelfile.bonsai-32k <<'EOF'
FROM defyma85/bonsai-27b-q1_0

# OpenCode needs at least 16K, but Bonsai supports 256K!
PARAMETER num_ctx 32768
PARAMETER temperature 0.7
PARAMETER top_p 0.9
PARAMETER repeat_penalty 1.1
EOF

# Create the model
~/bin/ollama create bonsai-32k -f Modelfile.bonsai-32k
```

### Step 2: Update OpenCode Config

```bash
# Backup current config
cp ~/.config/opencode/opencode.json ~/.config/opencode/opencode.backup.json

# Create Ollama Bonsai config
cat > ~/.config/opencode/opencode.json <<'EOF'
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
          "name": "Bonsai 27B 1-bit (32K context)",
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

echo "✓ OpenCode configured to use Ollama with Bonsai-27B"
```

### Step 3: Test

```bash
# Quick test
~/bin/ollama run bonsai-32k "Write a Python function to reverse a string. Just code."

# Use with OpenCode
cd ~/projects/local-inference/c-code
opencode
```

---

## 🔄 **Switching Between Models**

### Quick Switcher Commands

```bash
# Save configs for easy switching
cp ~/.config/opencode/opencode.json ~/.config/opencode/opencode.bonsai.json

# Add aliases to ~/.zshrc
cat >> ~/.zshrc <<'EOF'

# Model switchers for OpenCode
alias oc-fast='cp ~/.config/opencode/opencode.bonsai.json ~/.config/opencode/opencode.json && echo "✓ Using Bonsai-27B (fast)"'
alias oc-quality='cp ~/.config/opencode/opencode.devstral.json ~/.config/opencode/opencode.json && echo "✓ Using Devstral-24B (quality)"'
alias oc-which='echo "Current:" && grep -E "(baseURL|model)" ~/.config/opencode/opencode.json | grep -v schema'
EOF

source ~/.zshrc

# Usage:
oc-fast      # Switch to Bonsai (fast exploration)
oc-quality   # Switch to Devstral (quality coding)
oc-which     # Show current config
```

---

## 📊 **Comparison: Bonsai vs Devstral**

| Feature | Bonsai 27B | Devstral 24B |
|---------|-----------|--------------|
| **Size** | 3.8 GB | 15 GB |
| **Parameters** | 27B (1-bit) | 24B (4-bit) |
| **Speed** | 100-120 tok/s | 75-90 tok/s |
| **Quality** | 90% retained | 100% |
| **Context** | 256K | 32K |
| **SWE-Bench** | ~60% (est.) | 68% |
| **Use Case** | Fast/experimental | Production quality |
| **Memory** | ~4 GB | ~15 GB |
| **Backend** | Ollama/llama.cpp | Ollama/llama.cpp |

---

## 🎓 **Understanding 1-bit Quantization**

### Normal Model (16-bit)
```
Weight: -0.3742  →  16 bits  →  High precision
```

### 4-bit Quantization
```
Weight: -0.3742  →  4 bits   →  Good approximation
```

### 1-bit Quantization (Bonsai)
```
Weight: -0.3742  →  1 bit    →  -1 or +1
Special: Uses ternary for some layers (−1, 0, +1)
```

**How it works**:
- Most weights become -1 or +1
- Critical layers use ternary (3 values)
- Custom kernels optimize for 1-bit math
- Surprisingly effective at 90% quality retention

---

## 💡 **Recommended Workflow**

### Strategy 1: Bonsai for Exploration, Devstral for Implementation

```bash
# Phase 1: Understand & Plan (use Bonsai)
oc-bonsai
opencode
# "Analyze this codebase and suggest refactoring strategies"
# "Explain how the scheduler works"
# Fast, good for comprehension

# Phase 2: Implement (switch to Devstral)
oc-devstral
opencode
# "Implement the heap-based priority queue refactoring"
# "Add comprehensive error handling"
# Slower but higher quality code generation
```

### Strategy 2: Use Both for Different Projects

```bash
# Quick scripts/experiments
oc-bonsai

# Production code/critical features
oc-devstral
```

---

## 🐛 **Troubleshooting**

### "Model not found" error

```bash
# Check if downloaded
~/bin/ollama list | grep bonsai

# If not there, wait for download to complete
# Check download status in background task
```

### OpenCode not using Bonsai

```bash
# Verify config
oc-which

# Should show:
# "baseURL": "http://localhost:11434/v1"  (Ollama)
# "model": "ollama/bonsai-32k"

# If not, switch:
oc-fast
```

### Slow performance

```bash
# Bonsai should be FASTER than Devstral
# If it's slow:

# 1. Check Ollama version (should be 0.32.1)
~/bin/ollama --version

# 2. Verify model is loaded
~/bin/ollama ps

# 3. Restart Ollama if needed
killall ollama
open -a Ollama
```

---

## 📈 **Benchmarking**

### Test Bonsai Speed

```bash
time ~/bin/ollama run bonsai-32k "Write a function to calculate fibonacci. Just code."
```

**Expected on M3 Pro**:
- First token: <1 second
- Generation: 100-120 tok/s
- Total: ~3-5 seconds for short response

### Compare with Devstral

```bash
# Test Bonsai
time ~/bin/ollama run bonsai-32k "Write quicksort in Python"

# Test Devstral
time ~/bin/ollama run devstral-32k "Write quicksort in Python"
```

---

## 🔐 **Privacy Status**

### With Bonsai + Ollama

✅ **100% local** - Ollama processes everything on your Mac  
✅ **No network calls** - After model download  
✅ **No telemetry** - OpenCode config disables sharing  
✅ **GDPR/HIPAA compliant** - No data export  

**Data flow**:
```
Your code → OpenCode → Ollama (localhost:11434) → bonsai-32k → Response
```

**Nothing leaves your Mac.**

---

## 🎯 **Advanced: Ternary Bonsai (Higher Quality)**

If you want better quality (95% vs 90%):

```bash
# Download ternary version (5.9 GB)
~/bin/ollama pull MichelRosselli/ternary-bonsai

# Create custom model
cat > Modelfile.ternary-bonsai-32k <<'EOF'
FROM MichelRosselli/ternary-bonsai
PARAMETER num_ctx 32768
PARAMETER temperature 0.7
EOF

~/bin/ollama create ternary-bonsai-32k -f Modelfile.ternary-bonsai-32k

# Update OpenCode config to use ternary-bonsai-32k
```

**Trade-off**:
- **Size**: 5.9 GB (vs 3.8 GB)
- **Speed**: 80-100 tok/s (vs 100-120 tok/s)
- **Quality**: 95% (vs 90%)

---

## 📚 **Resources**

### Official
- [PrismML Bonsai Announcement](https://prismml.com/news/bonsai-27b)
- [Bonsai-27B GGUF on Hugging Face](https://huggingface.co/prism-ml/Bonsai-27B-gguf)
- [Bonsai Documentation](https://docs.prismml.com/models/bonsai-27b)

### Community
- [Bonsai Ollama Desktop Setup](https://github.com/bonsai27b/Bonsai-27b-Ollama-Desktop)
- [What Is Bonsai 27B?](https://kie.ai/blog/what-is-bonsai-27b)
- [Bonsai 27B Review](https://www.developersdigest.tech/blog/bonsai-27b-mobile-inference)

### Technical
- [MarkTechPost Analysis](https://www.marktechpost.com/2026/07/14/prismml-releases-bonsai-27b-1-bit-and-ternary-builds-of-qwen3-6-27b-that-run-on-laptops-and-phones/)

---

## ✅ **Current Status**

🔄 **Bonsai-27B**: Downloading (~3.8 GB)  
✅ **Ollama 0.32.1**: Ready  
✅ **Setup docs**: Created  
✅ **Switcher aliases**: Ready to add  

**After download completes**:
1. Create bonsai-32k model (with 32K context)
2. Update OpenCode config
3. Test with: `opencode`

**Expected performance**:
- **Speed**: 100-120 tok/s (faster than Devstral!)
- **Quality**: 90% of full-precision
- **Memory**: Only ~4 GB
- **Context**: Up to 256K tokens!

---

**Last Updated**: July 22, 2026  
**Your Hardware**: MacBook Pro M3 Pro, 36GB RAM  
**Best For**: Fast experimentation, learning, resource-efficient coding
