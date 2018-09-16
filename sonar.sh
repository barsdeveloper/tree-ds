#!/bin/sh

export PLATFORM=x86_64
build-wrapper-linux-x86-64 --out-dir ./build/bw_output make clean all
sonar-scanner \
  -Dsonar.projectKey=tree-ds \
  -Dsonar.organization=barsan-ds-github \
  -Dsonar.projectVersion=1.0 \
  -Dsonar.sources=./include \
  -Dsonar.cfamily.build-wrapper-output=./build/bw_output \
  -Dsonar.host.url=https://sonarcloud.io \
  -Dsonar.login=e28169718a4419456d1da1452392b20efbe40c6b \
  -Dsonar.coverageReportPaths=./build/coverage/kcov-merged/sonarqube.xml
