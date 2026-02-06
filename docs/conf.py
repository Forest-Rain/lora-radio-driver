# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

from sphinx.builders.html import StandaloneHTMLBuilder
import subprocess, os

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
]

source_suffix = {
    ".rst": "restructuredtext",
    ".md": "markdown",
}

templates_path = ['_templates']
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']

language = 'zh_CN'

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'sphinx_rtd_theme'

html_static_path = ['_static']

# --- Breathe configuration -----------------------------------------------------
breathe_projects = {
	"lora-radio-driver": "_build/xml/"
}
breathe_default_project = "lora-radio-driver"
breathe_default_members = ('members', 'undoc-members')
breathe_show_enumvalue_initializer = True