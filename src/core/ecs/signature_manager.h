#ifndef CORE_ECS_SIGNATURE_MANAGER_H
#define CORE_ECS_SIGNATURE_MANAGER_H

namespace core::ecs {

class SignatureManager {
public:
	static SignatureManager& GetInstance() {
		static SignatureManager instance;
		return instance;
	}

private:
	SignatureManager() = default;
};
} // namespace core::ecs

#endif // CORE_ECS_SIGNATURE_MANAGER_H
