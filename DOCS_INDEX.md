# Documentation Index

Quick reference to all documentation files in this directory.

---

## 🚀 **Start Here**

### [INSTALL.md](INSTALL.md) (NEW!)
**Complete installation guide** - Step-by-step setup from scratch

### [README.md](README.md)
**Quick start guide** - System overview, common workflows, quick reference

---

## 📖 **Core Guides**

### [OLLAMA_GUIDE.md](OLLAMA_GUIDE.md) (14K)
**Comprehensive Ollama reference**
- All Ollama commands (list, pull, create, run, etc.)
- Model management
- API usage
- Performance tuning
- Troubleshooting
- Your installed models reference

**Use when:** You need Ollama command syntax or troubleshooting

---

### [OPENCODE_REALITY.md](OPENCODE_REALITY.md) (11K)
**How to actually use multiple models with OpenCode**
- Why per-agent model config doesn't work
- Profile switching setup (oc-fast, oc-quality)
- Custom agents (alternative approach)
- Context switching in OpenCode
- Practical workflows

**Use when:** You want to understand or configure OpenCode model switching

---

### [SETUP_COMPLETE.md](SETUP_COMPLETE.md) (11K)
**Complete setup summary**
- What's installed and configured
- Performance comparisons
- Server management
- Troubleshooting
- Next steps

**Use when:** You need a comprehensive overview of your entire setup

---

## 🔧 **Specific Topics**

### [BONSAI_SETUP.md](BONSAI_SETUP.md) (9.6K)
**Bonsai 27B model guide**
- 1-bit quantization explained
- Performance expectations
- When to use Bonsai
- Setup instructions
- Comparison with Devstral

**Use when:** You want to understand the Bonsai model (`oc-fast`)

---

### [OPENCODE_PRIVACY.md](OPENCODE_PRIVACY.md) (13K)
**Privacy analysis**
- OpenCode data collection analysis
- Local vs cloud providers
- Privacy verification
- Network traffic analysis
- GDPR/HIPAA compliance

**Use when:** You need to verify privacy or explain to stakeholders

---

## 🎯 **Quick Reference**

### By Use Case

**"How do I...?"**
- Use Ollama commands → [OLLAMA_GUIDE.md](OLLAMA_GUIDE.md)
- Switch models in OpenCode → [OPENCODE_REALITY.md](OPENCODE_REALITY.md)
- Understand Bonsai → [BONSAI_SETUP.md](BONSAI_SETUP.md)
- Verify privacy → [OPENCODE_PRIVACY.md](OPENCODE_PRIVACY.md)
- Get quick overview → [README.md](README.md)
- See full setup details → [SETUP_COMPLETE.md](SETUP_COMPLETE.md)

### By Workflow

**Exploring codebase:**
```bash
oc-fast    # Uses Bonsai 27B
```
→ See [BONSAI_SETUP.md](BONSAI_SETUP.md) and [OPENCODE_REALITY.md](OPENCODE_REALITY.md)

**Writing code:**
```bash
oc-quality    # Uses Devstral 24B
```
→ See [OLLAMA_GUIDE.md](OLLAMA_GUIDE.md) and [OPENCODE_REALITY.md](OPENCODE_REALITY.md)

---

## 📊 **File Summary**

| File | Size | Type | Status |
|------|------|------|--------|
| README.md | 3K | Quick Start | ✅ Current |
| DOCS_INDEX.md | 2K | Navigator | ✅ Current |
| OLLAMA_GUIDE.md | 14K | Reference | ✅ Current |
| OPENCODE_REALITY.md | 10K | Guide | ✅ Current |
| SETUP_COMPLETE.md | 11K | Overview | ✅ Current |
| BONSAI_SETUP.md | 9.6K | Guide | ✅ Current |
| OPENCODE_PRIVACY.md | 13K | Analysis | ✅ Current |

**Total:** 7 files, ~63K of accurate, up-to-date documentation

---

## 🗑️ **Removed Files** (outdated)

These files were removed because they described systems that don't work or are no longer relevant:

### Initial Cleanup
- ❌ OLD_README.md - Described Ollama 0.13.5 + Qwen 7B setup
- ❌ THREE_TIER_SETUP.md - Per-agent model config that doesn't work
- ❌ OPENCODE_MULTI_MODEL_GUIDE.md - Incorrect agent model configuration
- ❌ DEVSTRAL_SETUP.md - Historical research doc
- ❌ INFERENCE_SPEED_GUIDE.md - Old vs new Ollama comparison
- ❌ OPENCODE_SETUP.md - PATH troubleshooting (resolved)

### Rapid-MLX Removal
- ❌ RAPID_MLX_OPENCODE.md - Rapid-MLX integration (not using)
- ❌ opencode.rapid.json - Rapid-MLX config (removed)

---

## 🎯 **Current System**

**Engine:** Ollama 0.32.1 (MLX support)

**Models:**
- Bonsai 27B (3.8 GB) - Fast exploration
- Devstral 24B (15 GB) - Quality coding
- Qwen 3B (2 GB) - Quick tasks

**Switching:** Profile-based (`oc-fast`, `oc-quality`)

**Privacy:** 100% local, no cloud calls

---

**Last Updated:** July 22, 2026  
**Status:** All documentation reflects current Ollama-only setup
