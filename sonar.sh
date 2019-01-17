#!/bin/sh

sonar-scanner \
  -Dsonar.projectKey=barsan-md_tree-ds \
  -Dsonar.organization=barsan-md-github \
  -Dsonar.projectVersion=1.0 \
  -Dsonar.sources=./include \
  -Dsonar.cfamily.build-wrapper-output=bw-output \
  -Dsonar.host.url=https://sonarcloud.io \
  -Dsonar.login=$(cat ./sonar_key) \
  -Dsonar.coverageReportPaths=./build/coverage/kcov-merged/sonarqube.xml

