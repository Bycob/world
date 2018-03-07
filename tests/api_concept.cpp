
int main() {
    // Creation du chunk system
    RegularChunkSystem chunkSystem;
    chunkSystem.setChunkSize(1000);
    chunkSystem.setChunkHeight(1000);
    chunkSystem.setFactor(0.5);

    // Creation du monde
    FlatWorld world(chunkSystem);

    // Configuration du sol
    PerlinGroundGenerator* groundGen = new PerlinGroundGenerator();

    // (On peut faire ça :
    // groundGen->createTerrain(resolution, sizeX, sizeY, sizeZ, NO_CONTEXT);
    // )

    groundGen.set(/* ... */);

    Ground& ground = world.getGround();
    ground.setTerrainGenerator(groundGen);

    // Ajout de forêts
    SimpleTreesDecorator* forest = new SimpleTreesDecorator();
    forest->setDensity(0.02);
    forest->setTreeGenerator(new TreeGenerator());

    world.addDecorator(forest);

    // C'est le moment de récupérer les fruits de notre génération !
    FlatWorldAssetsCollector collector;

    // A la première personne
    FirstPersonExplorer firstPerson;
    firstPerson.setMaxResolution(0.02);// 2 cm
    firstPerson.setDecreaseFactor(0.5); // On perd la moitié de notre résolution tous les xxx metres
    firstPerson.moveAt({0, 0, 0});

    firstPerson.explore(world, collector);

    // Récupération des assets
    collector.getAllTerrains();
    collector.getAllMeshes();
    collector.getTexture(texID);

    collector.reset();
    // En zone
    LocalExplorer local; // nom un peu nul
    local.setResolution(0.05); // 5 cm
    local.setBounds({-5, -5, -5}, {5, 5, 5});
    local.explore(world, collector);

    // Recuperation des assets...
}