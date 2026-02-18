#!/bin/bash

echo "=================================="
echo "   P2P File Transfer Server"
echo "=================================="
echo ""

# Check if Maven wrapper exists
if [ -f "./mvnw" ]; then
    echo "Starting with Maven Wrapper..."
    ./mvnw spring-boot:run
elif command -v mvn &> /dev/null; then
    echo "Starting with Maven..."
    mvn spring-boot:run
else
    echo "Error: Maven not found!"
    echo "Please install Maven or use the Maven wrapper."
    echo ""
    echo "To install Maven wrapper:"
    echo "  mvn -N wrapper:wrapper"
    exit 1
fi
