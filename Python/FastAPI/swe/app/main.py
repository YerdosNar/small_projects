from fastapi import FastAPI
from app.routers import auth, topics, comments

app = FastAPI(
    title="Campfire",
    description="A discussion app where topics burn out if they go cold.",
    version="0.1.0"
)

app.include_router(auth.router)
app.include_router(topics.router)
app.include_router(comments.router)


@app.get("/", tags=["Health"])
def root():
    return {"status": "🔥 Campfire is burning"}
