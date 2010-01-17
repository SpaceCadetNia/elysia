class SimpleProteinEnvironment {
  std::vector<ProteinDeposit> mProteins;
public:
  float getProteinDensity(const Vector3 &location, const Elysia::Genome::ProteinEffect&);
  std::vector<std::pair<Elysia::Genome::ProteinEffect, float> > getCompleteProteinDensity(const Vector3& location);
};