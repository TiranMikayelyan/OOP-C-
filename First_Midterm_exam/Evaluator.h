#pragma once
#include <memory>
#include "ast.h"
#include "registry.h"

double evaluateMatrix(std::shared_ptr<ASTNode> root, Registry& registry);