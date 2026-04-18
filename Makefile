PYTHON ?= python3
DOCKER_IMAGE ?= llvm-mcs51-local

.PHONY: bootstrap build check test docker-build docker-test clean

bootstrap:
	$(PYTHON) -m scripts.bootstrap_llvm

build:
	$(PYTHON) -m scripts.build_llvm

check: test

test:
	$(PYTHON) -m scripts.test

docker-build:
	docker build -t $(DOCKER_IMAGE) .

docker-test: docker-build
	docker run --rm -v "$$(pwd):/work" $(DOCKER_IMAGE) make test

clean:
	rm -rf out
