#!/usr/bin/env bash

# General aliases that could be global
git config alias.prepush 'log --graph --stat origin/master..'

# Staging aliases
stage_cmd="ssh git@vtk.org stage VES"
git_branch="\$(git symbolic-ref HEAD | sed -e 's|^refs/heads/||')"
git config alias.stage-cmd "!${stage_cmd}"
git config alias.stage-push "!bash -c \"git fetch stage --prune && git push stage HEAD\""
git config alias.stage-branch "!bash -c \"${stage_cmd} print\""
git config alias.stage-merge-next "!bash -c \"${stage_cmd} merge -b next ${git_branch}\""
git config alias.stage-merge-master "!bash -c \"${stage_cmd} merge -b master ${git_branch}\""
git config alias.stage-merge "!bash -c \"${stage_cmd} merge ${git_branch}\""
