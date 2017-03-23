#define _USE_MATH_DEFINES

#include "TreeGenerator.h"
#include "../mesh.h"
#include "../meshop.h"
#include "../maths/mathshelper.h"

#include <math.h>
#include <stdexcept>

using maths::vec3d;

TreeGenerator::TreeGenerator() {

}

TreeGenerator::~TreeGenerator() {

}

std::unique_ptr<Tree> TreeGenerator::generate() const {
	if (_skeletton == nullptr) {
		throw std::runtime_error("TreeGenerator : no default skeletton defined");
	}
	return std::move(generate(*_skeletton));
}

std::unique_ptr<Tree> TreeGenerator::generate(const TreeSkeletton & skeletton) const {
	std::unique_ptr<Tree> tree = std::make_unique<Tree>();

	//Création du mesh
	Mesh & trunkMesh = tree->_trunkMesh;
	auto primary = skeletton.getPrimaryNode();
	populateTrunkMesh(trunkMesh, primary, 0, primary->getWeight());

	MeshOps::recalculateNormals(trunkMesh);

	return std::move(tree);
}

void TreeGenerator::fillBezier(Mesh &trunkMesh, const BezierCurve & curve, int divCount,
	double startWeight, double endWeight, int mergePos) const {

	double segmentCountd = (double)_segmentCount;
	auto v0 = trunkMesh.getVertices(VType::POSITION).at(mergePos).getValues();
	vec3d c0 = curve.getPointAt(0);
	vec3d oldAx(v0[0] - c0.x, v0[1] - c0.y, v0[2] - c0.z);

	// Création du mesh selon la courbe de Bezier
	for (int i = 1; i <= divCount; i++) {
		const double t = (double)i / (double)divCount;
		const vec3d ptT = curve.getPointAt(t);
		const vec3d derivT = curve.getDerivativeAt(t);

		// Calcul de la largeur locale de la branche
		const double localRadius = (sqrt(endWeight) * t + sqrt(startWeight) * (1 - t)) * 0.15;

		// Calcul des vecteurs ax et ay dans le plan de la section de la branche
		vec3d ez(0, 0, 1);
		vec3d ax(1, 0, 0);
		vec3d ay(0, 1, 0);

		if (derivT.x != 0 || derivT.y != 0) {
			ay = ez.crossProduct(derivT).normalize();
			ax = ay.crossProduct(derivT).normalize();
		}

		// Calcul de l'offset avec la corole précédente
		double d = oldAx.crossProduct(ax).dotProduct(derivT);
		double offAngle = getAngle(oldAx, ax) * (abs(d) < 0.0001 ? 0 : d / abs(d));
		int offset = (int) (offAngle / (2 * M_PI)  * _segmentCount + 0.5) ;
		//std::cout << getAngle(oldAx, ax) << " " << offAngle << " " << offset << " " << oldAx << " " << ax << std::endl;

		// On ajoute un cercle de vertices
		for (int j = 0; j < _segmentCount; j++) {

			// A - Détermination de la position :
			double angle = (double)j / segmentCountd * 2 * M_PI;

			vec3d pt = ax * (cos(angle) * localRadius) + ay * (sin(angle) * localRadius);

			Vertex vert(VType::POSITION);
			vert.add(pt.x + ptT.x).add(pt.y + ptT.y).add(pt.z + ptT.z);

			// TODO C - Détermination de la coordonnée de texture


			// D - Ajout du vertex et de la face rectangle
			trunkMesh.addVertex(vert);

			// TODO construction plus claire peut-être.
			// j1 et j2 : utilisés pour lier le dernier cercle de vertices.
			int j1 = maths::mod(j + offset, _segmentCount);
			int j2 = maths::mod(j1 - 1, _segmentCount);

			// Les deux vertices appartenant au cercle traité actuellement.
			const int v1 = trunkMesh.getCount(VType::POSITION) - 1;
			const int v2 = j == 0 ? v1 - 1 + _segmentCount : v1 - 1;
			// Les deux vertices appartenant au cercle précédent.
			const int v4 = i == 1 ? mergePos + j1 : v1 - j + j1 - _segmentCount;
			const int v3 = i == 1 ? mergePos + j2 : v1 - j + j2 - _segmentCount;
			
			Face face1;
			face1.addVertex(v1);
			face1.addVertex(v2);
			face1.addVertex(v3);

			Face face2;
			face2.addVertex(v1);
			face2.addVertex(v3);
			face2.addVertex(v4);

			trunkMesh.addFace(face1);
			trunkMesh.addFace(face2);
		}

		oldAx = ax;
	}
}

void TreeGenerator::populateTrunkMesh(Mesh & trunkMesh, const Node<TreeInfo>* node,
	double mergeLenParent, double mergeWeight) const {
	
	auto getEndWeight = [](const std::vector<Node<TreeInfo> *> &children) -> double {
		if (children.size() == 0) return 0;

		double maxWeight = 0;
		for (auto child : children) {
			if (child->getWeight() > maxWeight) 
				maxWeight = child->getWeight();
		}
		return maxWeight;
	};

	/* On génére la branche, puis les joints avec les branches suivantes, ou le bout de la
	branche s'il n'y a pas de branches suivantes. */
	const Node<TreeInfo> * parent = node->getParent();
	auto children = node->getChildrenOrNeighboursAccess();
	const TreeInfo &info = node->getInfo();

	const vec3d parentPos = parent == nullptr ? node->getPosition() : parent->getPosition();
	const vec3d pos = node->getPosition();

	const double segmentCountd = (double)_segmentCount;

	// Création des faces du corps de la branche
	double mergeLenChild = _mergeSize;
	double length = info._size - mergeLenChild - mergeLenParent;

	//Calcul du poids moyen des enfants
	double avgChildrenWeight = getEndWeight(children);
	double mergeWeight1 = mergeWeight;
	
	if (length < 0) {
		mergeLenChild = info._size / 2.0;
		length = 0;
	}

	if (length > 0 || parent == nullptr) {
		// Calcul du nombre de divisions (en faces) de la branche en longueur
		int divider = (int) ceil(length / _faceSize);

		const double cosPhi = cos(info._phi);
		const double sinPhi = sin(info._phi);
		const double cosTheta = cos(info._theta);
		const double sinTheta = sin(info._theta);

		if (parent != nullptr) {

			// Création de la courbe de Bezier
			vec3d unitVector(sinPhi * cosTheta, sinPhi * sinTheta, cosPhi);
			vec3d pt1 = parentPos + unitVector * mergeLenParent;
			vec3d pt2 = parentPos + unitVector * (info._size - mergeLenChild - sqrt(avgChildrenWeight) * 0.15); // avgWeight = facteur pour réduire l'encombrement au niveau du merge
			vec3d dir1 = (pt2 - pt1) / 3.0;
			vec3d dir2 = dir1 * (-1.0);

			BezierCurve curve(pt1, pt2, dir1, dir2);
			
			// Obtention des poids
			const double w1 = mergeWeight1;
			const double w2 = avgChildrenWeight + (mergeLenChild / info._size) * (node->getWeight() - avgChildrenWeight);
			mergeWeight1 = w2;

			fillBezier(trunkMesh, curve, divider, w1, w2, trunkMesh.getCount(VType::POSITION) - _segmentCount);
		}
		else {
			const double localRadius = sqrt(node->getWeight()) * 0.15;

			for (int j = 0; j < _segmentCount; j++) {

				// A - Détermination de la position :
				double angle = (double)j / segmentCountd * 2 * M_PI;

				// Application de l'angle par rapport à l'axe z
				const double x0 = cos(angle) * localRadius;
				const double y0 = sin(angle) * localRadius;
				const double z1 = -x0 * sinPhi;

				// Application de l'angle par rapport à l'axe x
				const double x1 = x0 * cosPhi * cosTheta - y0 * sinTheta;
				const double y1 = x0 * cosPhi * sinTheta + y0 * cosTheta;

				Vertex vert(VType::POSITION);
				vert.add(x1 + pos.x).add(y1 + pos.y).add(z1 + pos.z);

				// TODO B - Détermination de la normale

				// TODO C - Détermination de la coordonnée de texture


				// D - Ajout du vertex et de la face rectangle
				trunkMesh.addVertex(vert);
			}
		}
	}

	// Création des faces pour la transition
	
	// -> Cas du premier étage
	if (info._size == 0) {
		for (auto child : children) {
			populateTrunkMesh(trunkMesh, child, 0, node->getWeight());
		}
		return;
	}

	// -> Cas des autres étages

	// On retient la position de la dernière corolle du parent pour lier tous les enfants.
	const int mark = trunkMesh.getCount(VType::POSITION) - _segmentCount;
	// Origine commune côté parent
	const vec3d parentSide = pos - (pos - parentPos) * (mergeLenChild / info._size);
	const vec3d parentDirection = (pos - parentPos) * (0.5 * mergeLenChild / info._size);

	for (const Node<TreeInfo> *child : children) {

		// Récupération des infos du noeud enfant
		const TreeInfo childInfo = child->getInfo();
		const vec3d childPos = child->getPosition();
		const double childSize = childInfo._size;
		
		// Calcul de certaines informations utiles
		const double newMergeLen = maths::min(childInfo._size / 2, (double) _mergeSize);
		const double avgChildChildrenWeight = getEndWeight(child->getChildrenOrNeighboursAccess());

		// Création de la courbe de Bezier
		const vec3d childSide = pos + (childPos - pos) * (newMergeLen / childInfo._size);
		const vec3d childDirection = (pos - childPos) * (0.5 * newMergeLen / childInfo._size);

		BezierCurve curve(parentSide, childSide, parentDirection, childDirection);

		//Calcul des poids
		const double w1 = mergeWeight1;
		const double w2 = child->getWeight() - (newMergeLen / childInfo._size) * (child->getWeight() - avgChildChildrenWeight);

		fillBezier(trunkMesh, curve, _mergeRes, w1, w2, mark);

		populateTrunkMesh(trunkMesh, child, newMergeLen, w2);
	}
}

