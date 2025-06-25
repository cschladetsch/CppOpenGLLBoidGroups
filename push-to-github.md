# How to Push to GitHub

## 1. Create a GitHub Repository
1. Go to https://github.com and log in
2. Click the "+" icon in the top right → "New repository"
3. Name it "CppLiquid" (or your preferred name)
4. Don't initialize with README (you already have one)
5. Click "Create repository"

## 2. Initialize Git and Push

```bash
# Initialize git in your project directory
cd /home/xian/local/CppLiquid
git init

# Add all files
git add .

# Create initial commit
git commit -m "Initial commit: C++23 liquid simulation with OpenGL"

# Add your GitHub repository as origin (replace USERNAME with your GitHub username)
git remote add origin https://github.com/USERNAME/CppLiquid.git

# Push to GitHub
git branch -M main
git push -u origin main
```

## Alternative: Using SSH (if you have SSH keys set up)
```bash
git remote add origin git@github.com:USERNAME/CppLiquid.git
```

## 3. If You Get Authentication Errors

### Option A: Personal Access Token (Recommended)
1. Go to GitHub → Settings → Developer settings → Personal access tokens → Tokens (classic)
2. Generate new token with "repo" scope
3. Use the token as your password when prompted

### Option B: GitHub CLI
```bash
# Install GitHub CLI if not already installed
# Then authenticate
gh auth login

# Push using gh
gh repo create CppLiquid --public --source=. --remote=origin --push
```

## 4. Verify
- Go to https://github.com/USERNAME/CppLiquid
- You should see all your files there

## 5. Future Updates
```bash
# After making changes
git add .
git commit -m "Description of changes"
git push
```