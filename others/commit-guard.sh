#/bin/bash
template_hooks_dir=$HOME/git-templates/hooks
if [[ ! -d "$template_hooks_dir" ]];then
	mkdir -p "$template_hooks_dir"
fi
cp -rf ./pre-commit "$template_hooks_dir/pre-commit"
#Any repositories called git init(first time or later) will copy files in init.templatedir to its own git directory.
#Use git init again for existing repository to apply this setting.
git config --global init.templatedir "$HOME/git-templates"

#Filter for global.Change value0...valuen to your customized word to filter.
git config --global filter..value0 your-words-to-filter-which-apply-to-all-repository
#Filter for certain git host,like github.com. Change value0...valuen to your custmized word.
git config --global filter.github.com.value0 your-words-to-filter-which-apply-to-repository-at-github.com