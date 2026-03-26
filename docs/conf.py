# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

from sphinx.builders.html import StandaloneHTMLBuilder
import subprocess, os
import re

# Run doxygen first
# read_the_docs_build = os.environ.get('READTHEDOCS', None) == 'True'
# if read_the_docs_build:
subprocess.call('doxygen doxyfile.doxy', shell=True)

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = 'lora-radio-driver'
copyright = '2026, forest'
author = 'forest'
release = 'v1.4.6'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = [
    'myst_parser',
    'breathe',
    'sphinx.ext.autosectionlabel',
    'sphinx.ext.viewcode',
    'sphinx.ext.intersphinx',
]

source_suffix = {
    ".rst": "restructuredtext",
    ".md": "markdown",
}

templates_path = ['_templates']
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']

language = 'zh_CN'

# 中文搜索配置
try:
    import jieba
    dict_path = os.path.join(os.path.dirname(__file__), '_static', 'dict.txt')
    if os.path.exists(dict_path):
        jieba.load_userdict(dict_path)
    html_search_language = 'zh'
    html_search_options = {
        'html_experimental_html_search': True,
        'lang': 'zh',
        'tokenizer': 'jieba.sphinx_zh'
    }
except ImportError:
    print("警告: jieba 未安装，中文搜索可能不准确。")
    pass

# 自动生成章节标签
autosectionlabel_prefix_document = True

# 跨项目引用配置
intersphinx_mapping = {
    'python': ('https://docs.python.org/3/', None),
    'sphinx': ('https://www.sphinx-doc.org/en/master/', None),
}

# 链接检查配置
linkcheck_anchors = False
linkcheck_ignore = [
    r'https://github.com/.*#.*',
]

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'sphinx_rtd_theme'

html_context = {
    "display_github": True,
    "github_user": "Forest-Rain",
    "github_repo": "lora-radio-driver",
    "github_version": "master/docs/",
}

html_static_path = ['_static']

# --- Breathe configuration -----------------------------------------------------
breathe_projects = {
	"lora-radio-driver": "_build/xml/"
}
breathe_default_project = "lora-radio-driver"
breathe_default_members = ('members', 'undoc-members')
breathe_show_enumvalue_initializer = True