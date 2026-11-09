@echo off
setlocal

REM Message de commit : argument passé au script, sinon message par défaut
set "MSG=%~1"
if "%MSG%"=="" set "MSG=Mise à jour automatique"

REM Vérifie qu'on est bien dans un dépôt Git
git rev-parse --is-inside-work-tree >nul 2>&1
if errorlevel 1 (
    echo Erreur : ce dossier n'est pas un depot Git.
    exit /b 1
)

echo Ajout des fichiers...
git add .

REM Commit uniquement s'il y a des changements
git diff --cached --quiet
if errorlevel 1 (
    echo Commit...
    git commit -m "%MSG%"
) else (
    echo Aucun changement a commit.
)

echo Push...
git push
if errorlevel 1 (
    echo Erreur lors du push.
    exit /b 1
)

echo Termine.
endlocal