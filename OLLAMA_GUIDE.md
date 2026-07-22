# Ollama Command Reference Guide

## 📖 **About Ollama**

Ollama is a local LLM runtime that wraps llama.cpp with a user-friendly interface. On macOS, it uses Apple's MLX framework for optimal performance on Apple Silicon.

**Version:** 0.32.1 (latest as of July 2026)  
**Backend:** llama.cpp with MLX support  
**Location:** `/Applications/Ollama.app` and `~/bin/ollama`

---

## 🚦 **Status & Health Checks**

### Check if Ollama is Running

```bash
# Method 1: Check loaded models (fastest)
~/bin/ollama ps

# Method 2: Check process
pgrep -fl ollama

# Method 3: API health check
curl -s http://localhost:11434/api/tags > /dev/null && echo "✅ Running" || echo "❌ Down"

# Method 4: Version check (also verifies it's working)
~/bin/ollama --version
```

### Check What Models Are Loaded

```bash
~/bin/ollama ps
```

**Output example:**
```
NAME                   ID              SIZE     PROCESSOR    UNTIL              
devstral-32k:latest    984c596b925a    19 GB    100% GPU     4 minutes from now
```

**Fields:**
- **NAME**: Model name
- **ID**: Unique model identifier
- **SIZE**: Memory usage
- **PROCESSOR**: GPU/CPU usage percentage
- **UNTIL**: When model will unload from memory (if inactive)

---

## 🎮 **Server Management**

### Start Ollama

```bash
# macOS (preferred - starts as app)
open -a Ollama

# Command line (runs in foreground)
~/bin/ollama serve

# Background mode
~/bin/ollama serve > /dev/null 2>&1 &
```

### Stop Ollama

```bash
# Graceful shutdown
killall ollama

# Force kill (if frozen)
killall -9 ollama

# Close app on macOS
osascript -e 'quit app "Ollama"'
```

### Restart Ollama

```bash
killall ollama && sleep 2 && open -a Ollama
```

---

## 📦 **Model Management**

### List All Models

```bash
# Full list
~/bin/ollama list

# Compact list (names only)
~/bin/ollama list | awk '{print $1}' | tail -n +2

# Specific model search
~/bin/ollama list | grep devstral

# Sort by size
~/bin/ollama list | sort -k2 -h
```

### Download/Pull Models

```bash
# Basic pull
~/bin/ollama pull model-name

# Specific tag
~/bin/ollama pull llama3.2:3b

# From custom registry
~/bin/ollama pull registry.example.com/model-name
```

**Examples:**
```bash
# Qwen Coder models
~/bin/ollama pull qwen2.5-coder:3b
~/bin/ollama pull qwen2.5-coder:7b
~/bin/ollama pull qwen2.5-coder:32b

# Llama models
~/bin/ollama pull llama3.2:3b
~/bin/ollama pull llama3.2:11b

# Devstral (coding)
~/bin/ollama pull devstral-small-2:24b

# Bonsai (1-bit quantization)
~/bin/ollama pull defyma85/bonsai-27b-q1_0
```

### Delete Models

```bash
# Delete specific model
~/bin/ollama rm model-name

# Delete with tag
~/bin/ollama rm qwen2.5-coder:3b

# Delete multiple models
~/bin/ollama rm model1 model2 model3
```

**Cleanup old models:**
```bash
# List models older than 30 days
~/bin/ollama list | awk 'NR>1 && /days ago/ {print $1}'

# Delete old unused models (careful!)
~/bin/ollama list | awk 'NR>1 && /days ago/ {print $1}' | xargs -I {} ~/bin/ollama rm {}
```

### Copy/Rename Models

```bash
~/bin/ollama cp source-model destination-model
```

**Example:**
```bash
~/bin/ollama cp qwen2.5-coder:7b my-custom-qwen
```

### Show Model Info

```bash
# Model details
~/bin/ollama show model-name

# Modelfile (configuration)
~/bin/ollama show model-name --modelfile

# License info
~/bin/ollama show model-name --license

# Parameters only
~/bin/ollama show model-name --parameters

# System prompt
~/bin/ollama show model-name --system
```

**Example:**
```bash
~/bin/ollama show devstral-32k
```

---

## 🔧 **Custom Model Creation**

### Create Model from Modelfile

```bash
~/bin/ollama create model-name -f /path/to/Modelfile
```

**Modelfile structure:**
```dockerfile
FROM base-model

# Parameters
PARAMETER num_ctx 32768
PARAMETER temperature 0.7
PARAMETER top_p 0.9
PARAMETER repeat_penalty 1.1

# System prompt
SYSTEM You are a helpful coding assistant.

# Messages (optional)
MESSAGE user What is your purpose?
MESSAGE assistant I help with coding tasks.
```

**Examples:**
```bash
# Create custom Devstral with 32K context
cat > Modelfile.devstral-32k <<'EOF'
FROM devstral-small-2:24b
PARAMETER num_ctx 32768
PARAMETER temperature 0.7
EOF

~/bin/ollama create devstral-32k -f Modelfile.devstral-32k

# Create custom Qwen for admin tasks
cat > Modelfile.qwen-admin <<'EOF'
FROM qwen2.5-coder:3b
PARAMETER num_ctx 32768
PARAMETER temperature 0.7
SYSTEM You are a helpful assistant for administrative tasks.
EOF

~/bin/ollama create qwen-admin-32k -f Modelfile.qwen-admin
```

---

## 💬 **Interactive Usage**

### Run Model Interactively

```bash
# Start chat session
~/bin/ollama run model-name

# With specific prompt
~/bin/ollama run model-name "Your question here"

# Multiline mode
~/bin/ollama run model-name
```

**Interactive commands:**
- `/bye` - Exit session
- `/clear` - Clear screen
- `/save name` - Save session
- `/load name` - Load session
- `/set parameter value` - Change parameter
- `/show` - Show model info
- `"""` - Start/end multiline input

**Examples:**
```bash
# Quick question
~/bin/ollama run devstral-32k "Write a Python function to reverse a string"

# Chat session
~/bin/ollama run qwen-admin-32k
>>> How do I list files in a directory?
>>> /bye
```

### One-Shot Commands

```bash
# Get single response
~/bin/ollama run model-name "prompt" --verbose

# JSON output
~/bin/ollama run model-name "prompt" --format json

# No streaming (wait for complete response)
~/bin/ollama run model-name "prompt" --no-stream
```

---

## 🔌 **API Usage**

### Generate Completion

```bash
curl http://localhost:11434/api/generate -d '{
  "model": "devstral-32k",
  "prompt": "Write a function to add two numbers",
  "stream": false
}'
```

### Chat Completion

```bash
curl http://localhost:11434/api/chat -d '{
  "model": "devstral-32k",
  "messages": [
    {
      "role": "user",
      "content": "Write a Python function to reverse a string"
    }
  ],
  "stream": false
}'
```

### Extract Response

```bash
# Using jq
curl -s http://localhost:11434/api/generate -d '{
  "model": "devstral-32k",
  "prompt": "Say hello",
  "stream": false
}' | jq -r '.response'

# Using Python
curl -s http://localhost:11434/api/generate -d '{
  "model": "devstral-32k",
  "prompt": "Say hello",
  "stream": false
}' | python3 -c "import sys, json; print(json.load(sys.stdin)['response'])"
```

### List Models via API

```bash
curl -s http://localhost:11434/api/tags | jq '.models[] | .name'
```

### Check Model Status via API

```bash
curl -s http://localhost:11434/api/ps
```

---

## ⚙️ **Configuration & Parameters**

### Common Modelfile Parameters

```dockerfile
# Context window (tokens)
PARAMETER num_ctx 32768

# Temperature (0.0-1.0, default 0.8)
# Lower = more focused, higher = more creative
PARAMETER temperature 0.7

# Top-p sampling (0.0-1.0, default 0.9)
PARAMETER top_p 0.9

# Top-k sampling (default 40)
PARAMETER top_k 40

# Repeat penalty (default 1.1)
# Higher = less repetition
PARAMETER repeat_penalty 1.1

# Number of predictions to return
PARAMETER num_predict 128

# Stop sequences
PARAMETER stop "<|end|>"
PARAMETER stop "### "

# Random seed (for reproducibility)
PARAMETER seed 42

# Number of GPUs to use
PARAMETER num_gpu 1

# Number of threads
PARAMETER num_thread 8

# Keep model in memory
PARAMETER keep_alive 5m
```

### Environment Variables

```bash
# Model storage location
export OLLAMA_MODELS="$HOME/.ollama/models"

# Host/port
export OLLAMA_HOST="127.0.0.1:11434"

# Number of parallel requests
export OLLAMA_NUM_PARALLEL=4

# Max loaded models
export OLLAMA_MAX_LOADED_MODELS=1

# Keep models loaded
export OLLAMA_KEEP_ALIVE="5m"

# Debug mode
export OLLAMA_DEBUG=1

# Flash attention (auto/true/false)
export OLLAMA_FLASH_ATTENTION=auto
```

**Add to `~/.zshrc`:**
```bash
export OLLAMA_MODELS="$HOME/.ollama/models"
export OLLAMA_KEEP_ALIVE="10m"
export OLLAMA_NUM_PARALLEL=2
```

---

## 📊 **Performance & Monitoring**

### Monitor Resource Usage

```bash
# Watch loaded models
watch -n 1 '~/bin/ollama ps'

# Monitor memory usage
~/bin/ollama ps | awk 'NR>1 {print $3}' | paste -sd+ | bc

# Check GPU usage (if available)
~/bin/ollama ps | grep "GPU"
```

### Benchmark Model Speed

```bash
# Time a simple request
time ~/bin/ollama run devstral-32k "Write a Python function to add two numbers. Just code."

# Measure tokens/second
~/bin/ollama run devstral-32k "Count to 100" --verbose 2>&1 | grep "eval rate"
```

### Clear GPU Memory

```bash
# Unload all models
~/bin/ollama stop

# Force restart
killall ollama && sleep 2 && open -a Ollama
```

---

## 🧹 **Maintenance**

### Disk Space Management

```bash
# Check model storage size
du -sh ~/.ollama/models

# List models by size
~/bin/ollama list | tail -n +2 | sort -k2 -h

# Remove unused blobs (careful!)
# Ollama stores shared layers, so this can break models
# Only run if you know what you're doing
rm -rf ~/.ollama/models/blobs/sha256-*

# Safer: just remove old unused models
~/bin/ollama list | awk 'NR>1 && /months ago/ {print $1}' | xargs -I {} ~/bin/ollama rm {}
```

### Update Ollama

```bash
# Download latest installer
curl -fsSL https://ollama.com/install.sh | sh

# Or download from website
open https://ollama.com/download

# Verify version
~/bin/ollama --version
```

### Backup Models

```bash
# Backup model directory
cp -r ~/.ollama/models ~/.ollama/models.backup

# Backup specific model
~/bin/ollama show model-name --modelfile > model-name.Modelfile

# Restore from Modelfile
~/bin/ollama create model-name -f model-name.Modelfile
```

---

## 🐛 **Troubleshooting**

### Model Won't Load

```bash
# Check if model exists
~/bin/ollama list | grep model-name

# Try pulling again
~/bin/ollama pull model-name

# Check logs
~/Library/Logs/Ollama/server.log
```

### Slow Performance

```bash
# Check what's loaded
~/bin/ollama ps

# Unload models
~/bin/ollama stop

# Restart Ollama
killall ollama && sleep 2 && open -a Ollama

# Check CPU/GPU usage
top -o cpu | head -20
```

### Connection Refused

```bash
# Check if running
pgrep ollama

# Start if not running
open -a Ollama

# Check port
lsof -i :11434

# Verify API
curl http://localhost:11434/api/tags
```

### Out of Memory

```bash
# Check loaded models
~/bin/ollama ps

# Use smaller quantization
~/bin/ollama pull model-name:3b    # Instead of :7b or :32b

# Reduce context window in Modelfile
PARAMETER num_ctx 8192  # Instead of 32768

# Unload models
~/bin/ollama stop
```

### Corrupt Model

```bash
# Remove and re-download
~/bin/ollama rm model-name
~/bin/ollama pull model-name

# Clear cache
rm -rf ~/.ollama/models/manifests
```

---

## 🔗 **Integration with OpenCode**

### OpenCode Configuration

```json
{
  "provider": {
    "ollama": {
      "npm": "@ai-sdk/openai-compatible",
      "options": {
        "baseURL": "http://localhost:11434/v1"
      },
      "models": {
        "devstral-32k": {
          "name": "Devstral Small 2 24B",
          "contextWindow": 32768
        }
      }
    }
  },
  "model": "ollama/devstral-32k"
}
```

### Verify OpenCode Can Access Ollama

```bash
# Test API endpoint
curl http://localhost:11434/v1/models

# Test with OpenCode format
curl -s http://localhost:11434/v1/chat/completions -H "Content-Type: application/json" -d '{
  "model": "devstral-32k",
  "messages": [{"role": "user", "content": "Hello"}]
}' | jq '.choices[0].message.content'
```

---

## 📚 **Useful Aliases**

Add to `~/.zshrc`:

```bash
# Ollama status
alias ollama-status='~/bin/ollama ps 2>/dev/null && echo "✅ Ollama is running" || echo "❌ Ollama is not running"'

# Quick model list
alias ollama-ls='~/bin/ollama list'

# What's loaded
alias ollama-loaded='~/bin/ollama ps'

# Unload all
alias ollama-clear='~/bin/ollama stop && echo "All models unloaded"'

# Restart
alias ollama-restart='killall ollama && sleep 2 && open -a Ollama && echo "Ollama restarted"'

# Model sizes
alias ollama-sizes='~/bin/ollama list | tail -n +2 | awk "{print \$2, \$1}" | sort -h'

# Quick test
alias ollama-test='~/bin/ollama run qwen-admin-32k "Say hello in one word"'
```

---

## 🎯 **Your Current Models**

```bash
# List all models
~/bin/ollama list
```

**Installed models:**

| Model | Size | Purpose | Context |
|-------|------|---------|---------|
| `qwen-admin-32k` | 2 GB | Admin tasks (title, compaction) | 32K |
| `bonsai-32k` | 3.8 GB | Code exploration | 256K |
| `devstral-32k` | 15 GB | Main coding | 393K |
| `qwen2.5-coder:3b` | 2 GB | Base model (admin) | 32K |
| `qwen2.5-coder:7b` | 4.7 GB | Base model (general) | 32K |
| `defyma85/bonsai-27b-q1_0` | 3.8 GB | Base model (1-bit) | 262K |
| `devstral-small-2:24b` | 15 GB | Base model (coding) | 393K |

---

## 📖 **Quick Reference Card**

```bash
# Status
~/bin/ollama ps                          # What's loaded
~/bin/ollama list                        # All models
~/bin/ollama --version                   # Version

# Model Management
~/bin/ollama pull model-name             # Download
~/bin/ollama rm model-name               # Delete
~/bin/ollama create name -f Modelfile    # Create custom
~/bin/ollama show model-name             # Info

# Running
~/bin/ollama run model-name              # Interactive
~/bin/ollama run model-name "prompt"     # One-shot
~/bin/ollama stop                        # Unload all

# Server
open -a Ollama                           # Start (macOS)
killall ollama                           # Stop
pgrep ollama                             # Check process

# API
curl http://localhost:11434/api/tags     # List models
curl http://localhost:11434/api/ps       # Status
```

---

## 🔐 **Privacy & Security**

### Data Storage

```bash
# Models stored here:
~/.ollama/models/

# Logs stored here:
~/Library/Logs/Ollama/

# No telemetry by default
# All processing is local
```

### Network

```bash
# Ollama only listens on localhost by default
lsof -i :11434

# To expose to network (not recommended for security)
export OLLAMA_HOST="0.0.0.0:11434"
~/bin/ollama serve
```

### Privacy Status

✅ **100% local processing**  
✅ **No cloud API calls** (after model download)  
✅ **No telemetry**  
✅ **No data collection**  
✅ **GDPR/HIPAA compliant** (no data export)

---

## 📚 **Resources**

### Official
- [Ollama Website](https://ollama.com)
- [Ollama GitHub](https://github.com/ollama/ollama)
- [Model Library](https://ollama.com/library)
- [API Documentation](https://github.com/ollama/ollama/blob/main/docs/api.md)

### Community
- [Ollama Discord](https://discord.gg/ollama)
- [Reddit r/ollama](https://reddit.com/r/ollama)
- [Awesome Ollama](https://github.com/awesome-ollama/awesome-ollama)

---

**Last Updated:** July 22, 2026  
**Ollama Version:** 0.32.1  
**Your System:** macOS, Apple Silicon (MLX optimized)  
**Installation:** `/Applications/Ollama.app`, `~/bin/ollama`
