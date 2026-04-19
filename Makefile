PYTHON ?= python3

.PHONY: bootstrap build check test clean

bootstrap:
	$(PYTHON) -m scripts.bootstrap_llvm

build:
	$(PYTHON) -m scripts.build_llvm

check: test

test:
	$(PYTHON) -m scripts.test

clean:
	rm -rf out
