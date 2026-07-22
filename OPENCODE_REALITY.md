# OpenCode Multi-Model Reality

How to **actually** use multiple models with OpenCode on your Mac.

---

## 🎯 **What Actually Works**

**Profile Switching** - The only reliable way to use different models with OpenCode.

```bash
# Fast mode (Bonsai 27B)
oc-fast
opencode

# Quality mode (Devstral 24B)
oc-quality
opencode
```

That's it. No per-agent configs, no complex routing. Just swap profiles.

---

## ❌ **What Doesn't Work**

### Per-Agent Model Configuration

You might think you can configure different models for different OpenCode agents:

```json
{
  "agent": {
    "general": { "model": "ollama/devstral-32k" },
    "explore": { "model": "ollama/bonsai-32k" },
    "plan": { "model": "ollama/bonsai-32k" }
  }
}
```

**This doesn't work.** Built-in OpenCode agents ignore per-agent model settings.

### Why Not?

**GitHub Issue #21952**: Built-in agents (general, explore, plan, build, etc.) do not respect per-agent model configuration. They all use the global `model` setting.

The `agent` field in `opencode.json` only applies to **custom agents** you create yourself, not the built-in ones.

---

## ✅ **Profile Switching: How It Works**

### The Concept

Instead of trying to configure multiple models in one config, maintain **separate config files** and swap between them.

### Setup

**Step 1: Create Profile Configs**

```bash
# Fast profile (Bonsai 27B)
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

# Quality profile (Devstral 24B)
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
```

**Step 2: Add Switcher Aliases**

Add to `~/.zshrc`:

```bash
# OpenCode model switchers
alias oc-fast='cp ~/.config/opencode/opencode.fast.json ~/.config/opencode/opencode.json && echo "✓ Fast mode: Bonsai 27B (100-120 tok/s)"'
alias oc-quality='cp ~/.config/opencode/opencode.quality.json ~/.config/opencode/opencode.json && echo "✓ Quality mode: Devstral 24B (75-90 tok/s)"'
alias oc-which='echo "Current model:" && grep -A1 "\"model\":" ~/.config/opencode/opencode.json | tail -1'
```

**Step 3: Reload Shell**

```bash
source ~/.zshrc
```

### Usage

```bash
# Switch to fast mode
oc-fast

# Check current model
oc-which
# Output: "model": "ollama/bonsai-32k"

# Start OpenCode (now using Bonsai)
cd ~/projects/your-project
opencode

# When done, switch to quality mode
oc-quality

# Start OpenCode (now using Devstral)
opencode
```

---

## 🔄 **Context Switching in OpenCode**

OpenCode provides several ways to manage context within a session:

### 1. Sessions (`/sessions` command)

Start fresh conversations without losing history:

```bash
opencode
# Do some work...

# Start a new session
/new

# Later, list all sessions
/sessions

# Switch back to previous session
/switch 1
```

**Use when**: You want to work on different tasks in the same terminal but keep conversations separate.

### 2. Multiple Terminal Windows

Run OpenCode in different terminals simultaneously:

```bash
# Terminal 1
cd ~/project-a
opencode

# Terminal 2
cd ~/project-b
opencode
```

**Use when**: Working on multiple projects at once, each needs different context.

### 3. Project-Specific Configs

Override global config with local `.opencode/opencode.json`:

```bash
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
cd ~/my-project
opencode
```

**Use when**: A specific project always needs the same model.

---

## 📊 **Comparison: Profile Switching vs Custom Agents**

| Approach | Built-in Agents | Custom Agents | Complexity | Reliability |
|----------|----------------|---------------|------------|-------------|
| **Profile switching** | ✅ Works | ✅ Works | Low | High |
| **Per-agent config** | ❌ Ignored | ✅ Works | High | Medium |

**Recommendation**: Use profile switching unless you're building custom agents.

---

## 💡 **Practical Workflows**

### Workflow 1: Fast Exploration → Quality Implementation

```bash
# Phase 1: Understand codebase (use Bonsai)
oc-fast
cd ~/my-project
opencode
# "Explain how authentication works"
# "Show me all database-related files"
# Fast responses, good comprehension

# Phase 2: Implement changes (switch to Devstral)
oc-quality
opencode
# "Add comprehensive error handling to auth system"
# "Refactor database connection pooling"
# Slower but higher quality code generation
```

### Workflow 2: Different Projects, Different Models

```bash
# Quick scripts (Bonsai)
oc-fast
cd ~/scripts
opencode

# Production code (Devstral)
oc-quality
cd ~/production-app
opencode
```

### Workflow 3: Time-Based Switching

```bash
# Morning: Exploration phase
oc-fast
# Work 2-3 hours exploring codebase, planning

# Afternoon: Implementation phase
oc-quality
# Work on actual code changes
```

---

## 🔍 **How to Verify What Model Is Being Used**

### Method 1: Check Config

```bash
oc-which
# Shows: "model": "ollama/bonsai-32k"
```

### Method 2: Watch Ollama

```bash
# In another terminal
watch -n 1 '~/bin/ollama ps'
```

When OpenCode makes requests, you'll see which model loads.

### Method 3: Speed Test

Bonsai should be noticeably faster (100-120 tok/s) than Devstral (75-90 tok/s).

---

## 🐛 **Troubleshooting**

### "OpenCode still using old model after switching"

OpenCode caches config on startup. After switching profiles:

```bash
# Exit OpenCode
/exit

# Switch profile
oc-quality

# Start OpenCode again
opencode
```

### "Can't tell which model is active"

```bash
# Check current config
oc-which

# Verify Ollama
~/bin/ollama ps

# Test speed
# Ask OpenCode something
# Bonsai: 100-120 tok/s
# Devstral: 75-90 tok/s
```

### "Profile switching not working"

```bash
# Verify configs exist
ls -la ~/.config/opencode/opencode.*.json

# Check aliases
alias | grep oc-

# Reload shell
source ~/.zshrc

# Try again
oc-fast
oc-which
```

---

## 📚 **Reference: GitHub Issue #21952**

**Title**: "Built-in agents don't respect per-agent model configuration"

**Status**: Known limitation as of OpenCode 1.18.x

**Workaround**: Use profile switching or create custom agents

**Links**:
- [GitHub Issue](https://github.com/opencodeai/opencode/issues/21952)
- [Custom Agent Documentation](https://docs.opencode.ai/guides/custom-agents)

---

## ✅ **What You Should Remember**

1. **Built-in agents ignore per-agent model config** - Don't waste time trying to configure them
2. **Profile switching works perfectly** - Use `oc-fast` and `oc-quality`
3. **Context switching is separate** - Use `/new`, `/sessions`, or multiple terminals
4. **Project-specific configs are possible** - Put `.opencode/opencode.json` in project root
5. **Always verify** - Use `oc-which` to confirm active model

---

**Last Updated**: July 22, 2026  
**OpenCode Version**: 1.18.0+  
**Based on**: Real testing and GitHub Issue #21952
