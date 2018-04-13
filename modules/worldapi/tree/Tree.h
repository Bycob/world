#pragma once

#include "core/WorldConfig.h"

#include <memory>

#include "core/WorldObject.h"
#include "assets/Mesh.h"
#include "assets/Material.h"
#include "ITreeWorker.h"
#include "TreeSkeletton.h"

namespace world {

	class PTree;

	class WORLDAPI_EXPORT Tree : public WorldObject {
	public:
		Tree();

		~Tree() override;

		void setup(const Tree &model);

		template <typename T, typename...Args>
		T &addWorker(Args&&... args);

		const TreeSkeletton &getSkeletton() const;

		TreeSkeletton &getSkeletton();

		const Mesh &getTrunkMesh() const;

		Mesh &getTrunkMesh();

		void collectWholeObject(ICollector &collector) override;

	private :
        PTree *_internal;

		TreeSkeletton _skeletton;
		Mesh _trunkMesh;
		Material _trunkMaterial;

        bool _generated = false;

        void addWorkerInternal(ITreeWorker * worker);

        void generateBase();

		friend class TrunkGenerator;
	};

	template <typename T, typename...Args>
	T & Tree::addWorker(Args &&... args) {
		T * worker = new T(args...);
		addWorkerInternal(worker);
		return *worker;
	}
}
